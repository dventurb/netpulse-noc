#include "connectivity_controller.h"

#include "connectivity_view.h"
#include "connectivity_worker.h"

#include "persistence.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void connectivity_controller_init(connectivity_controller_t *controller, connectivity_view_t *view, void *data)
{
  controller->view = view;
  controller->app = (application_t *)data;

  controller->ip[0] = '\0';

  controller->source = SOURCE_SEARCH;
  controller->selected_equipment = NULL;
}

void connectivity_controller_ping(connectivity_controller_t *controller, const char *count, const char *timeout, const char *packet_size)
{
  ping_params_t *params = malloc(sizeof(ping_params_t));
  if (params == NULL) return;

  snprintf(params->ip, IP_MAX, "%s", controller->ip);
  params->count = atoi(count);
  params->timeout = atoi(timeout);
  params->packet_size = atoi(packet_size);

  ping_task_worker(params, on_ping_finished, controller);
}

void connectivity_controller_search_equipment(connectivity_controller_t *controller, const char *text)
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
    connectivity_controller_set_equipment(controller, &node->data);
    ping_view_update_list(&controller->view->ping_view, &node->data, 1);
  }

  else 
    ping_view_update_list(&controller->view->ping_view, NULL, 0);
}

ping_validation_t connectivity_controller_validate_ping(const char *ip, const char *count, const char *timeout, const char *packet_size)
{
  if (validate_ip_address(ip) == FALSE) 
    return PING_INVALID_IP_ADDRESS;

  if (validate_ping_count(atoi(count)) == FALSE)
    return PING_INVALID_COUNT;

  if (validate_ping_timeout(atoi(timeout)) == FALSE)
    return PING_INVALID_TIMEOUT;

  if (validate_ping_packet_size(atoi(packet_size)) == FALSE)
    return PING_INVALID_PACKET_SIZE;

  return PING_OK;
}

void connectivity_controller_set_equipment(connectivity_controller_t *controller, equipment_t *equipment)
{
  if (equipment == NULL) return;

  controller->selected_equipment = equipment;
}

void connectivity_controller_set_source_selection(connectivity_controller_t *controller, target_source_t source)
{
  controller->source = source;
}

void connectivity_controller_set_ip_address(connectivity_controller_t *controller, const char *ip_address)
{
  snprintf(controller->ip, IP_MAX, "%s", ip_address);
}

void connectivity_controller_set_ip_from_source(connectivity_controller_t *controller, const char *ip_address)
{
  if (controller->source == SOURCE_MANUAL)
  {
    connectivity_controller_set_ip_address(controller, ip_address);
  }
}

static incident_priority_t incident_controller_get_priority(equipment_type_t type)
{
  switch (type)
  {
    case TYPE_ROUTER: 
    case TYPE_FIREWALL:
    case TYPE_SERVER:
      return PRIORITY_CRITICAL;

    case TYPE_SWITCH:
    case TYPE_NAS:
    case TYPE_UPS:
      return PRIORITY_HIGH;

    case TYPE_ACCESS_POINT:
    case TYPE_IP_CAMERA:
    case TYPE_PRINTER:
    case TYPE_OTHER:
      return PRIORITY_MEDIUM;
    default:
      return PRIORITY_LOW;
  }
}

void connectivity_controller_create_incident(connectivity_controller_t *controller, const equipment_t *equipment)
{
  incident_queue_t *queue = &controller->app->incidents_pending;
  incident_list_t *list = &controller->app->incidents_history;

  incident_t new;

  new.source_type = SOURCE_EQUIPMENT;
  equipment_format_id(equipment->id, new.source_id);

  snprintf(new.type, STRING_MAX, "%s", "Offline");
  snprintf(new.description, DESCRIPTION_MAX, "Device [%s] did not respond to ping.", equipment->name);

  snprintf(new.technician_name, STRING_MAX, "%s", "TODO: current_user");

  new.priority = incident_controller_get_priority(equipment->type);

  incident_queue_enqueue(queue, new);

  save_incidents(queue, list, "data/incidents.bin");
}

gboolean on_ping_finished(gpointer data)
{
  ping_task_t *task = (ping_task_t *)data;

  if (task->result->responded == false && task->controller->source == SOURCE_SEARCH)
  {
    equipment_update_status(task->controller->selected_equipment, STATUS_FAILED);
    equipment_update_last_check(task->controller->selected_equipment);
   
    connectivity_controller_create_incident(task->controller, task->controller->selected_equipment);

    save_equipments(&task->controller->app->equipments, "data/equipments.bin");
  }

  else if (task->result->responded == true && task->controller->source == SOURCE_SEARCH)
  {
    equipment_update_status(task->controller->selected_equipment, STATUS_OPERATIONAL);
    equipment_update_last_check(task->controller->selected_equipment);

    save_equipments(&task->controller->app->equipments, "data/equipments.bin");
  }

  ping_view_set_result(&task->controller->view->ping_view, task->result);

  free(task->params); // Controller
  free(task->result); // Model 
  free(task); // Worker

  return false;
}
