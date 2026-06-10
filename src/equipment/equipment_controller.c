#include "equipment_controller.h"

#include "equipment_worker.h"
#include "equipment_view.h"

#include "persistence.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void equipment_controller_init(equipment_controller_t *controller, equipment_view_t *view, void *data)
{
  controller->view = view;
  controller->app = (application_t *)data;

  controller->selected_count = 0;
  controller->selected_node = NULL;

  controller->pagination.page = 0;
  controller->pagination.page_size = 6;
 
  controller->pagination.total = equipment_get_count(&controller->app->equipments);

  controller->status_filter = 0;
  controller->type_filter = 0;

  controller->sort = 0;
  controller->prev_sort = 0;
}

static void equipment_controller_dispatch(equipment_controller_t *controller)
{
  equipment_params_t *params = malloc(sizeof(equipment_params_t));
  if (params == NULL) return;

  params->start = pagination_start(controller->pagination);
  params->end = pagination_end(controller->pagination);

  params->status_filter = controller->status_filter;
  params->type_filter = controller->type_filter;

  params->sort = controller->sort;
  params->prev_sort = controller->prev_sort;

  equipment_task_worker(params, on_equipment_finish, controller);
}

void equipment_controller_refresh_page(equipment_controller_t *controller)
{
  controller->selected_count = 0;
  controller->selected_node = NULL;
  controller->pagination.page = 0;
  
  controller->pagination.total = equipment_get_count(&controller->app->equipments);

  controller->status_filter = 0;
  controller->type_filter = 0;

  equipment_controller_update_table(controller);
}

void equipment_controller_update_table(equipment_controller_t *controller)
{
  equipment_controller_dispatch(controller);
}

void equipment_controller_apply_filters(equipment_controller_t *controller, int sort, int status, int type)
{
  controller->status_filter = status;
  controller->type_filter = type;

  controller->sort = sort;

  controller->pagination.page = 0;

  equipment_controller_update_table(controller);
}

void equipment_controller_add(equipment_controller_t *controller, equipment_t data)
{
  equipment_list_t *list = &controller->app->equipments;
  equipment_node_t *node = equipment_list_insert(list, data);

  char id[ID_MAX];
  equipment_format_id(node->data.id, id);

  hashmap_insert(&controller->app->id_index, id, node);
  hashmap_insert(&controller->app->ip_index, node->data.ip_address, node);
  hashmap_insert(&controller->app->mac_index, node->data.mac_address, node);

  controller->selected_node = NULL;
  controller->selected_count = 0;

  equipment_controller_update_table(controller);

  save_equipments(list, "data/equipments.bin");
}

void equipment_controller_edit(equipment_controller_t *controller, equipment_t data)
{
  equipment_list_t *list = &controller->app->equipments;

  equipment_node_t *node = controller->selected_node;
  if (node == NULL) return;

  if (strcmp(node->data.ip_address, data.ip_address) != 0)
    hashmap_update(&controller->app->ip_index, node->data.ip_address, data.ip_address, node);

  if (strcmp(node->data.mac_address, data.mac_address) != 0)
    hashmap_update(&controller->app->mac_index, node->data.mac_address, data.mac_address, node);
  
  equipment_update(&node->data, data);

  controller->selected_node = NULL;
  controller->selected_count = 0;

  equipment_controller_update_table(controller);

  save_equipments(list, "data/equipments.bin");
}

void equipment_controller_remove(equipment_controller_t *controller)
{
  equipment_list_t *list = &controller->app->equipments;
  
  equipment_node_t *node = controller->selected_node;
  if (node == NULL) return;

  char id[ID_MAX];
  equipment_format_id(node->data.id, id);

  hashmap_remove(&controller->app->id_index, id);
  hashmap_remove(&controller->app->ip_index, node->data.ip_address);
  hashmap_remove(&controller->app->mac_index, node->data.mac_address);

  equipment_list_remove(list, node);

  controller->selected_node = NULL;
  controller->selected_count = 0;

  equipment_controller_update_table(controller);

  save_equipments(list, "data/equipments.bin");
}

void equipment_controller_search(equipment_controller_t *controller, const char *text)
{
  if (text == NULL || strlen(text) == 0) return;

  equipment_node_t *node = NULL;

  char buffer[strlen(text) + 1];
  convert_to_uppercase(text, buffer);

  switch (detect_search_type(buffer)) 
  {
    case SEARCH_EQUIPMENT_ID:
      node = (equipment_node_t *) hashmap_get(&controller->app->id_index, buffer);
      break;
    case SEARCH_IP:
      node = (equipment_node_t *) hashmap_get(&controller->app->ip_index, buffer);
      break;
    case SEARCH_MAC:
      node = (equipment_node_t *) hashmap_get(&controller->app->mac_index, buffer);
      break;
    default:
      break;
  }

  if (node != NULL) 
  {
    controller->pagination.page = 0;
    controller->pagination.total = 1;

    equipment_view_update_table(controller->view, &node->data, 1);
  }

  else 
  {
    controller->pagination.page = 0;
    controller->pagination.total = 0;

    equipment_view_update_table(controller->view, NULL, 0);
  }
}

void equipment_controller_handle_toggled(equipment_controller_t *controller, int id, bool is_active)
{
  hashmap_t *id_index = &controller->app->id_index;

  char buffer[ID_MAX];
  equipment_format_id(id, buffer);

  equipment_node_t *node = (equipment_node_t *)hashmap_get(id_index, buffer);
  if (node == NULL) return;

  // TODO: selected multiple rows 
  if (is_active == TRUE)
  {
    controller->selected_node = node;
    controller->selected_count++;
  }

  else 
  {
    controller->selected_node = NULL;
    controller->selected_count--;
  }

  equipment_view_update_header(controller->view);
}

void equipment_controller_get_stats(equipment_controller_t *controller, equipment_stats_t *stats)
{
  stats->total = equipment_get_count(&controller->app->equipments);
  stats->operational = equipment_get_number_status(&controller->app->equipments, STATUS_OPERATIONAL);
  stats->failed = equipment_get_number_status(&controller->app->equipments, STATUS_FAILED);
  stats->maintenance = equipment_get_number_status(&controller->app->equipments, STATUS_MAINTENANCE);
}

gboolean on_equipment_finish(gpointer data)
{
  equipment_task_t *task = (equipment_task_t *)data;

  task->controller->pagination.total = task->total;

  int total_pages = pagination_total_pages(task->controller->pagination, task->total);

  if (task->controller->pagination.page >= total_pages - 1)
    task->controller->pagination.page = total_pages - 1;

  equipment_view_update_table(task->controller->view, task->result, task->count);
  equipment_view_update_stats_cards(task->controller->view);
  equipment_view_update_header(task->controller->view);

  free(task->params);
  free(task->result);
  free(task);

  return false;
}
