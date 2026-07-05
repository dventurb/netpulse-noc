#include "ping_controller.h"

#include "ping_view.h"
#include "ping_worker.h"

#include "terminal_panel.h"

#include "persistence.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void ping_controller_init(ping_controller_t *controller, ping_view_t *view, app_data_t *data)
{
  controller->view = view;
  controller->data = data;

  controller->ip[0] = '\0';

  controller->source = SOURCE_SEARCH;
  controller->selected_equipment = NULL;
}

void ping_controller_run_ping(ping_controller_t *controller, const char *count, const char *timeout, const char *packet_size)
{
  ping_params_t *params = malloc(sizeof(ping_params_t));
  if (params == NULL) return;

  snprintf(params->ip, IP_MAX, "%s", controller->ip);
  params->count = atoi(count);
  params->timeout = atoi(timeout);
  params->packet_size = atoi(packet_size);

  ping_single_task_worker(params, on_run_ping_finished, controller);
}

void ping_controller_run_ping_all_equipments(ping_controller_t *controller)
{
  int count = equipment_get_count(&controller->data->equipments);
  if (count == 0) 
  {
    ping_view_set_actions_enabled(controller->view, true);
    return;
  }

  ping_params_t *params = malloc(sizeof(ping_params_t));
  if (params == NULL) return;

  params->ip[0] = '\0';
  params->count = 1;
  params->timeout = 1;
  params->packet_size = 56;

  ping_all_task_worker(params, on_run_ping_finished, controller);
}

void ping_controller_search_equipment(ping_controller_t *controller, const char *text)
{
  if (text == NULL || strlen(text) == 0) return;

  equipment_node_t *node = NULL;

  char buffer[strlen(text) + 1];
  convert_to_uppercase(text, buffer);

  switch (detect_search_type(buffer)) 
  {
    case SEARCH_EQUIPMENT_ID:
      node = (equipment_node_t *) hashmap_get(&controller->data->id_index, buffer);
      break;
    case SEARCH_IP:
      node = (equipment_node_t *) hashmap_get(&controller->data->ip_index, buffer);
      break;
    case SEARCH_MAC:
      node = (equipment_node_t *) hashmap_get(&controller->data->mac_index, buffer);
      break;
    default:
      break;
  }

  if (node != NULL) 
  {
    ping_controller_set_equipment(controller, &node->data);
    ping_view_update_list(controller->view, &node->data, 1);
  }

  else 
    ping_view_update_list(controller->view, NULL, 0);
}

void ping_controller_set_source_selection(ping_controller_t *controller, target_source_t source)
{
  controller->source = source;
}

void ping_controller_set_equipment(ping_controller_t *controller, equipment_t *equipment)
{
  if (equipment == NULL) return;

  controller->selected_equipment = equipment;
}

void ping_controller_set_ip_address(ping_controller_t *controller, const char *ip_address)
{
  snprintf(controller->ip, IP_MAX, "%s", ip_address);
}

void ping_controller_set_ip_from_source(ping_controller_t *controller, const char *ip_address)
{
  if (controller->source == SOURCE_MANUAL)
  {
    ping_controller_set_ip_address(controller, ip_address);
    controller->selected_equipment = NULL;
  }
}

void ping_controller_save_file(ping_controller_t *controller, const char *filepath)
{
  if (filepath == NULL) return;

  char *text = terminal_panel_get_output(&controller->view->terminal);
  if (text == NULL) return;

  FILE *file = fopen(filepath, "w");
  if (file == NULL)
  {
    g_free(text);
    return;
  }

  fprintf(file, "%s", text);
  
  fclose(file);
  g_free(text);
}

ping_validation_t ping_controller_validate_fields(const char *ip, const char *count, const char *timeout, const char *packet_size)
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

void ping_controller_create_equipment_incident(ping_controller_t *controller, const equipment_t *equipment)
{
  incident_queue_t *queue = &controller->data->incidents_pending;
  incident_list_t *list = &controller->data->incidents_history;

  incident_t new;

  new.source_type = SOURCE_EQUIPMENT;
  equipment_format_id(equipment->id, new.source_id);

  snprintf(new.type, STRING_MAX, "%s", "Offline");
  snprintf(new.description, DESCRIPTION_MAX, "Device [%s] did not respond to ping.", equipment->name);

  snprintf(new.technician_name, STRING_MAX, "%s", controller->data->current_user->name);

  new.priority = incident_get_priority(SOURCE_EQUIPMENT, equipment->type);

  incident_queue_enqueue(queue, new);

  save_incidents(queue, list);
}

void ping_controller_get_stats(ping_controller_t *controller, ping_stats_t *stats)
{
  ping_result_t *result = controller->result;

  snprintf(stats->status, STRING_MAX, "%s", result->responded ? "Online" : "Offline");

  snprintf(stats->loss_value, STRING_MAX, "%d%%", result->packets_loss_percent);
  snprintf(stats->loss_subtitle, STRING_MAX, "%d transmitted, %d received", result->packets_sent, result->packets_received);

  snprintf(stats->latency, STRING_MAX, "%.1f", result->avg_latency);

  char datetime[DATETIME_MAX];
  format_timestamp_to_datetime(time(NULL), datetime);

  snprintf(stats->execution_value, STRING_MAX, "%s", datetime);
  snprintf(stats->execution_subtitle, STRING_MAX, "Duration: %.2fs", result->duration);
}

gboolean on_run_ping_finished(gpointer data)
{
  ping_task_t *task = (ping_task_t *)data;
  
  terminal_panel_set_output(&task->controller->view->terminal, task->result->input);
  terminal_panel_set_output(&task->controller->view->terminal, task->result->output);

  task->controller->result = task->result;

  ping_view_update_cards(task->controller->view);
  ping_view_set_actions_enabled(task->controller->view, true);

  free(task->params); // Controller
  free(task->result); // Model 
  free(task); // Worker

  return false;
}

gboolean on_run_ping_all_equipments_finished(gpointer data)
{
  ping_controller_t *controller = (ping_controller_t *)data;

  terminal_panel_set_output(&controller->view->terminal, controller->result->input);
  terminal_panel_set_output(&controller->view->terminal, controller->result->output);

  return false;
}
