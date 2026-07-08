#include "incident_controller.h"

#include "incident_worker.h"
#include "incident_view.h"

#include "persistence.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void incident_controller_init(incident_controller_t *controller, incident_view_t *view, app_data_t *data)
{
  controller->view = view;
  controller->data = data;

  controller->selected_count = 0;
  controller->selected_node = NULL;

  int total = incident_get_count(&controller->data->incidents_pending, &controller->data->incidents_history);
  pagination_init(&controller->pagination, total);

  controller->status_filter = 0;
  controller->priority_filter = 0;

  controller->search_text[0] = '\0';
}

static void incident_controller_dispatch(incident_controller_t *controller)
{
  incident_params_t *params = malloc(sizeof(incident_params_t));
  if (params == NULL) return;

  params->start = pagination_start(controller->pagination);
  params->end = pagination_end(controller->pagination);

  params->status_filter = controller->status_filter;
  params->priority_filter = controller->priority_filter;

  snprintf(params->search_text, STRING_MAX, "%s", controller->search_text);

  incident_task_worker(params, on_incident_finished, controller);
}

void incident_controller_refresh_page(incident_controller_t *controller)
{
  controller->selected_count = 0;
  controller->selected_node = NULL;

  int total = incident_get_count(&controller->data->incidents_pending, &controller->data->incidents_history);
  pagination_init(&controller->pagination, total);

  controller->status_filter = 0;
  controller->priority_filter = 0;

  incident_controller_update_table(controller);
}

void incident_controller_update_table(incident_controller_t *controller)
{
  incident_controller_dispatch(controller);
}

void incident_controller_add(incident_controller_t *controller, incident_t data)
{
  incident_queue_t *queue = &controller->data->incidents_pending;
  incident_list_t *list = &controller->data->incidents_history;
  
  incident_queue_enqueue(queue, data);

  controller->selected_node = NULL;
  controller->selected_count = 0;

  incident_controller_update_table(controller);

  save_incidents(queue, list);
}

void incident_controller_process(incident_controller_t *controller)
{
  incident_queue_t *queue = &controller->data->incidents_pending;
  incident_list_t *list = &controller->data->incidents_history;

  incident_node_t *node = incident_queue_dequeue(queue);
  if (node == NULL) return;

  incident_list_insert(list, node);

  incident_controller_update_table(controller);

  save_incidents(queue, list);
}

void incident_controller_resolve(incident_controller_t *controller)
{
  if (controller->selected_node == NULL) return;

  incident_queue_t *queue = &controller->data->incidents_pending;
  incident_list_t *list = &controller->data->incidents_history;

  if (controller->selected_node->data.status == INCIDENT_IN_PROGRESS)
  {
    controller->selected_node->data.status = INCIDENT_CONCLUDED;
    controller->selected_node->data.concluded_at = time(NULL);
  }

  controller->selected_node = NULL;
  controller->selected_count = 0;

  incident_controller_update_table(controller);

  save_incidents(queue, list);
}

void incident_controller_apply_filters(incident_controller_t *controller, int status, int priority)
{
  controller->status_filter = status;
  controller->priority_filter = priority;

  controller->pagination.current_page = 0;

  incident_controller_update_table(controller);
}

void incident_controller_search(incident_controller_t *controller, const char *text)
{
  if (text == NULL) return;

  char buffer[strlen(text) + 1];
  convert_to_uppercase(text, buffer);

  // TODO: Search by Technician Name and Equipment/Sensor ID
  switch (detect_search_type(buffer)) 
  {
    case SEARCH_INCIDENT_ID:
      snprintf(controller->search_text, ID_MAX, "%s", buffer);
      controller->search_type = SEARCH_INCIDENT_ID;
      break;
    case SEARCH_EQUIPMENT_ID:
      snprintf(controller->search_text, ID_MAX, "%s", buffer);
      controller->search_type = SEARCH_EQUIPMENT_ID;
      break;
    case SEARCH_SENSOR_CODE:
      snprintf(controller->search_text, CODE_MAX, "%s", buffer);
      controller->search_type = SEARCH_SENSOR_CODE;
      break;
    default:
      controller->search_text[0] = '\0';
      controller->search_type = SEARCH_INVALID;
      break;
  }

  controller->pagination.current_page = 0;

  incident_controller_update_table(controller);
}

void incident_controller_handle_toggled(incident_controller_t *controller, int id, bool is_active)
{
  incident_queue_t *queue = &controller->data->incidents_pending;
  incident_list_t *list = &controller->data->incidents_history;

  incident_node_t *node = incident_queue_get_by_id(queue, id);
  if (node == NULL)
    node = incident_list_get_by_id(list, id);

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

  incident_view_update_header(controller->view);
}

int incident_controller_get_position(incident_controller_t *controller, incident_t incident, int row)
{
  int position = 0; 

  if (incident.status == INCIDENT_PENDING)
    position = controller->pagination.current_page * 6 + row;
    
  return position;
}

incident_validation_t incident_controller_validate(incident_controller_t *controller, incident_t incident)
{
  hashmap_t *hashmap = &controller->data->id_index;

  search_type_t source_type = detect_search_type(incident.source_id);

  if (source_type != SEARCH_EQUIPMENT_ID && 
    source_type != SEARCH_SENSOR_CODE)
    return INCIDENT_INVALID_SOURCE_ID;

  if (source_type == SEARCH_EQUIPMENT_ID && 
      hashmap_get(hashmap, incident.source_id) == NULL)
      return INCIDENT_INVALID_SOURCE_ID;
  
  if (strlen(incident.type) < TYPE_MIN) 
    return INCIDENT_INVALID_TYPE;

  if (strlen(incident.description) <= DESCRIPTION_MIN) 
    return INCIDENT_INVALID_DESCRIPTION;

  return INCIDENT_VALID;
}

void incident_controller_get_stats(incident_controller_t *controller, incident_stats_t *stats)
{
  int total = incident_get_count(&controller->data->incidents_pending, &controller->data->incidents_history);
  snprintf(stats->total, STRING_MAX, "%d", total);

  int pending = incident_queue_get_count(&controller->data->incidents_pending);
  snprintf(stats->pending, STRING_MAX, "%d", pending);

  int in_progress = incident_list_get_number_status(&controller->data->incidents_history, INCIDENT_IN_PROGRESS);
  snprintf(stats->in_progress, STRING_MAX, "%d", in_progress);

  int concluded = incident_list_get_number_status(&controller->data->incidents_history, INCIDENT_CONCLUDED);
  snprintf(stats->concluded, STRING_MAX, "%d", concluded);
}

gboolean on_incident_finished(gpointer data)
{
  incident_task_t *task = (incident_task_t *)data;

  task->controller->pagination.total_items = task->total;

  int total_pages = pagination_total_pages(task->controller->pagination, task->total);

  pagination_fix_current_page(&task->controller->pagination, total_pages);

  incident_view_update_table(task->controller->view, task->result, task->count);
  incident_view_update_stats_cards(task->controller->view);
  incident_view_update_header(task->controller->view);

  free(task->params);
  free(task->result);
  free(task);

  return false;
}
