#include "controller.h"

#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "persistence.h"
#include "utils.h"
#include "ui_inventory.h"
#include "ui_connectivity.h"
#include "ui_incident.h"

gboolean on_ping_finished(gpointer data)
{
  ping_task_t *task = (ping_task_t *)data;

  ui_ping_set_result(task->ui_ping, task->result);

  free(task->params); // Controller
  free(task->result); // Model 
  free(task); // Worker

  return false;
}

gboolean on_equipment_finished(gpointer data)
{
  equipment_task_t *task = (equipment_task_t *)data;

  task->ui_inventory->controller.pagination.total = pagination_total_pages(task->ui_inventory->controller.pagination, task->count);

  if (task->ui_inventory->controller.pagination.page >= task->ui_inventory->controller.pagination.total)
    task->ui_inventory->controller.pagination.page = task->ui_inventory->controller.pagination.total - 1;

  ui_inventory_update_table(task->ui_inventory, task->result, task->count);

  free(task->params);
  free(task->result);
  free(task);

  return false;
}

gboolean on_incident_finished(gpointer data)
{
  incident_task_t *task = (incident_task_t *)data;

  task->ui_incident->controller.pagination.total = pagination_total_pages(task->ui_incident->controller.pagination, task->count);

  if (task->ui_incident->controller.pagination.page >= task->ui_incident->controller.pagination.total)
    task->ui_incident->controller.pagination.page = task->ui_incident->controller.pagination.total - 1;

  ui_incident_update_table(task->ui_incident, task->result, task->count);

  free(task->params);
  free(task->result);
  free(task);

  return false;
}

void connectivity_controller_ping(ui_ping_configuration_t *ui_ping, const char *ip, int count, int timeout, int packet_size)
{
  ping_params_t *params = malloc(sizeof(ping_params_t));
  if (params == NULL) return;

  strncpy(params->ip, ip, STRING_MAX - 1);
  params->ip[STRING_MAX - 1] = '\0';
  params->count = count;
  params->timeout = timeout;
  params->packet_size = packet_size;

  ping_task_worker(params, on_ping_finished, ui_ping);
}

void pagination_controller_get_range(pagination_t pagination, int *start, int *end)
{
  *start = pagination.page - 1;
  *end = pagination.page + 1;

  int total = pagination.total - 1;

  if (*start < 0) *start = 0;
  if (*end > total) *end = total;
}

void pagination_controller_page_number(pagination_t *pagination, int number)
{
  pagination->page = number;
}

void pagination_controller_previous(pagination_t *pagination)
{
  pagination->page--;

  if (pagination->page < 0) 
    pagination->page = 0; 
}

void pagination_controller_next(pagination_t *pagination)
{
  pagination->page++;

  if (pagination->page > pagination->total - 1) 
    pagination->page = pagination->total - 1; 
}

static void equipment_controller_dispatch(ui_inventory_t *ui_inventory)
{
  equipment_params_t *params = malloc(sizeof(equipment_params_t));
  if (params == NULL) return;

  params->start = pagination_start(ui_inventory->controller.pagination);
  params->end = pagination_end(ui_inventory->controller.pagination);

  params->status_filter = ui_inventory->controller.status_filter;
  params->type_filter = ui_inventory->controller.type_filter;

  equipment_task_worker(params, on_equipment_finished, ui_inventory);
}

static void incident_controller_dispatch(ui_incident_t *ui_incident)
{
  incident_params_t *params = malloc(sizeof(incident_params_t));
  if (params == NULL) return;

  params->start = pagination_start(ui_incident->controller.pagination);
  params->end = pagination_end(ui_incident->controller.pagination);

  params->status_filter = ui_incident->controller.status_filter;
  params->priority_filter = ui_incident->controller.priority_filter;

  incident_task_worker(params, on_incident_finished, ui_incident);
}

void equipment_controller_refresh_page(ui_inventory_t *ui_inventory)
{
  ui_inventory->controller.selected_count = 0;
  ui_inventory->controller.selected_node = NULL;
  ui_inventory->controller.pagination.page = 0;
  ui_inventory->controller.pagination.total = pagination_total_pages(ui_inventory->controller.pagination, ui_inventory->application->equipments.count);

  ui_inventory->controller.status_filter = 0;
  ui_inventory->controller.type_filter = 0;

  equipment_controller_dispatch(ui_inventory);

  ui_inventory_update_stats_cards(ui_inventory);
  ui_inventory_update_header(ui_inventory);
}

void incident_controller_refresh_page(ui_incident_t *ui_incident)
{
  ui_incident->controller.selected_count = 0;
  ui_incident->controller.selected_node = NULL;
  ui_incident->controller.pagination.page = 0;
  ui_incident->controller.pagination.total = pagination_total_pages(ui_incident->controller.pagination, ui_incident->application->equipments.count);

  ui_incident->controller.status_filter = 0;
  ui_incident->controller.priority_filter = 0;

  incident_controller_dispatch(ui_incident);

  ui_incident_update_stats_cards(ui_incident);
  ui_incident_update_header(ui_incident);
}

void equipment_controller_update_table(ui_inventory_t *ui_inventory)
{
  equipment_controller_dispatch(ui_inventory);
}

void incident_controller_update_table(ui_incident_t *ui_incident)
{
  incident_controller_dispatch(ui_incident);
}

void equipment_controller_apply_filters(ui_inventory_t *ui_inventory, int status, int type)
{
  ui_inventory->controller.status_filter = status;
  ui_inventory->controller.type_filter = type;

  ui_inventory->controller.pagination.page = 0;

  equipment_controller_dispatch(ui_inventory);
}

void equipment_controller_add(ui_inventory_t *ui_inventory, equipment_t data)
{
  equipment_list_t *list = &ui_inventory->application->equipments;
  equipment_node_t *node = equipment_list_insert(list, data);

  char id[ID_MAX];
  equipment_format_id(node->data.id, id);

  hashmap_insert(&ui_inventory->application->id_index, id, node);
  hashmap_insert(&ui_inventory->application->ip_index, node->data.ip_address, node);
  hashmap_insert(&ui_inventory->application->mac_index, node->data.mac_address, node);

  ui_inventory->controller.pagination.total = pagination_total_pages(ui_inventory->controller.pagination, ui_inventory->application->equipments.count);

  if (ui_inventory->controller.pagination.page >= ui_inventory->controller.pagination.total)
    ui_inventory->controller.pagination.page = ui_inventory->controller.pagination.total - 1;

  if (ui_inventory->controller.pagination.page < 0)
    ui_inventory->controller.pagination.page = 0;

  equipment_controller_dispatch(ui_inventory);
  ui_inventory_update_stats_cards(ui_inventory);

  save_equipments(list, "data/equipments.bin");
}

void equipment_controller_edit(ui_inventory_t *ui_inventory, equipment_node_t *node, equipment_t data)
{
  if (node == NULL) return;
  
  equipment_list_t *list = &ui_inventory->application->equipments;

  if (strcmp(node->data.ip_address, data.ip_address) != 0)
    hashmap_update(&ui_inventory->application->ip_index, node->data.ip_address, data.ip_address, node);

  if (strcmp(node->data.mac_address, data.mac_address) != 0)
    hashmap_update(&ui_inventory->application->mac_index, node->data.mac_address, data.mac_address, node);
  
  equipment_update(&node->data, data);

  equipment_controller_dispatch(ui_inventory);
  ui_inventory_update_stats_cards(ui_inventory);

  save_equipments(list, "data/equipments.bin");
}

void equipment_controller_remove(ui_inventory_t *ui_inventory, equipment_node_t *node)
{
  if (node == NULL) return;

  equipment_list_t *list = &ui_inventory->application->equipments;

  char id[ID_MAX];
  equipment_format_id(node->data.id, id);

  hashmap_remove(&ui_inventory->application->id_index, id);
  hashmap_remove(&ui_inventory->application->ip_index, node->data.ip_address);
  hashmap_remove(&ui_inventory->application->mac_index, node->data.mac_address);

  equipment_list_remove(list, node);

  ui_inventory->controller.pagination.total = pagination_total_pages(ui_inventory->controller.pagination, ui_inventory->application->equipments.count);

  if (ui_inventory->controller.pagination.page >= ui_inventory->controller.pagination.total)
    ui_inventory->controller.pagination.page = ui_inventory->controller.pagination.total - 1;

  if (ui_inventory->controller.pagination.page < 0)
    ui_inventory->controller.pagination.page = 0;

  equipment_controller_dispatch(ui_inventory);
  ui_inventory_update_stats_cards(ui_inventory);

  save_equipments(list, "data/equipments.bin");
}

void equipment_controller_search(ui_inventory_t *ui_inventory, const char *text)
{
  if (text == NULL || strlen(text) == 0)
  {
    equipment_controller_refresh_page(ui_inventory);
    return;
  }

  equipment_node_t *node = NULL;

  char buffer[strlen(text) + 1];
  convert_to_uppercase(text, buffer);

  switch (detect_search_type(buffer)) 
  {
    case SEARCH_ID:
      node = (equipment_node_t *) hashmap_get(&ui_inventory->application->id_index, buffer);
      break;
    case SEARCH_IP:
      node = (equipment_node_t *) hashmap_get(&ui_inventory->application->ip_index, buffer);
      break;
    case SEARCH_MAC:
      node = (equipment_node_t *) hashmap_get(&ui_inventory->application->mac_index, buffer);
      break;
    case SEARCH_INVALID:
      break;
  }

  if (node != NULL) 
  {
    ui_inventory->controller.pagination.page = 0;
    ui_inventory->controller.pagination.total = 1;

    ui_inventory_update_table(ui_inventory, &node->data, 1);
  }

  else 
  {
    ui_inventory->controller.pagination.page = 0;
    ui_inventory->controller.pagination.total = 0;

    ui_inventory_update_table(ui_inventory, NULL, 0);
  }
}

void equipment_controller_handle_toggled(ui_inventory_t *ui_inventory, int id, bool is_active)
{
  hashmap_t *id_index = &ui_inventory->application->id_index;

  char buffer[ID_MAX];
  equipment_format_id(id, buffer);

  equipment_node_t *node = (equipment_node_t *)hashmap_get(id_index, buffer);
  if (node == NULL) return;

  // TODO: selected multiple rows 
  if (is_active == TRUE)
  {
    ui_inventory->controller.selected_node = node;
    ui_inventory->controller.selected_count++;
  }

  else 
  {
    ui_inventory->controller.selected_node = NULL;
    ui_inventory->controller.selected_count--;
  }

  ui_inventory_update_header(ui_inventory);
}

void incident_controller_add(ui_incident_t *ui_incident, incident_t data)
{
  incident_queue_t *queue = &ui_incident->application->incidents_pending;
  incident_list_t *list = &ui_incident->application->incidents_history;
  
  incident_queue_enqueue(queue, data);

  int total = incident_get_count(&ui_incident->application->incidents_pending, &ui_incident->application->incidents_history);

  ui_incident->controller.pagination.total = pagination_total_pages(ui_incident->controller.pagination, total);

  if (ui_incident->controller.pagination.page >= ui_incident->controller.pagination.total)
    ui_incident->controller.pagination.page = ui_incident->controller.pagination.total - 1;

  if (ui_incident->controller.pagination.page < 0)
    ui_incident->controller.pagination.page = 0;

  ui_incident_refresh(ui_incident);

  save_incidents(queue, list, "data/incident.bin");
}

void incident_controller_process(ui_incident_t *ui_incident)
{
  incident_queue_t *queue = &ui_incident->application->incidents_pending;
  incident_list_t *list = &ui_incident->application->incidents_history;

  incident_node_t *node = incident_queue_dequeue(queue);
  if (node == NULL) return;

  incident_list_insert(list, node);

  ui_incident_refresh(ui_incident);
}

void incident_controller_resolve(ui_incident_t *ui_incident)
{
  ui_incident->controller.selected_node->data.status = INCIDENT_CONCLUDED;
  ui_incident->controller.selected_node->data.concluded_at = time(NULL);

  ui_incident_refresh(ui_incident);
}

void incident_controller_apply_filters(ui_incident_t *ui_incident, int status, int priority)
{
  ui_incident->controller.status_filter = status;
  ui_incident->controller.priority_filter = priority;

  ui_incident->controller.pagination.page = 0;

  incident_controller_dispatch(ui_incident);
}

void incident_controller_search(ui_incident_t *ui_incident, const char *text)
{

}
void incident_controller_handle_toggled(ui_incident_t *ui_incident, int id, bool is_active)
{

}
