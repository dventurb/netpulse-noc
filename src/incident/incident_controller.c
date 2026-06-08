#include "incident_controller.h"

#include "incident_worker.h"
#include "incident_view.h"

#include "persistence.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void incident_controller_init(incident_controller_t *controller, incident_view_t *view, void *data)
{
  controller->view = view;
  controller->app = (application_t *)data;

  controller->selected_count = 0;
  controller->selected_node = NULL;

  controller->pagination.page = 0;
  controller->pagination.page_size = 6;

  int total = incident_get_count(&controller->app->incidents_pending, &controller->app->incidents_history);
  controller->pagination.total = pagination_total_pages(controller->pagination, total);

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
  controller->pagination.page = 0;

  int total = incident_get_count(&controller->app->incidents_pending, &controller->app->incidents_history);

  controller->pagination.total = pagination_total_pages(controller->pagination, total);

  controller->status_filter = 0;
  controller->priority_filter = 0;

  incident_controller_dispatch(controller);

  incident_view_update_stats_cards(controller->view);

  incident_view_update_header(controller->view);
}

void incident_controller_update_table(incident_controller_t *controller)
{
  incident_controller_dispatch(controller);
}

void incident_controller_add(incident_controller_t *controller, incident_t data)
{
  incident_queue_t *queue = &controller->app->incidents_pending;
  incident_list_t *list = &controller->app->incidents_history;
  
  incident_queue_enqueue(queue, data);

  int total = incident_get_count(&controller->app->incidents_pending, &controller->app->incidents_history);

  controller->pagination.total = pagination_total_pages(controller->pagination, total);

  if (controller->pagination.page >= controller->pagination.total)
    controller->pagination.page = controller->pagination.total;

  if (controller->pagination.page < 0)
    controller->pagination.page = 0;

  incident_controller_refresh_page(controller);

  save_incidents(queue, list, "data/incidents.bin");
}

void incident_controller_process(incident_controller_t *controller)
{
  incident_queue_t *queue = &controller->app->incidents_pending;
  incident_list_t *list = &controller->app->incidents_history;

  incident_node_t *node = incident_queue_dequeue(queue);
  if (node == NULL) return;

  incident_list_insert(list, node);

  incident_controller_refresh_page(controller);

  save_incidents(queue, list, "data/incidents.bin");
}

void incident_controller_resolve(incident_controller_t *controller)
{
  incident_queue_t *queue = &controller->app->incidents_pending;
  incident_list_t *list = &controller->app->incidents_history;

  controller->selected_node->data.status = INCIDENT_CONCLUDED;
  controller->selected_node->data.concluded_at = time(NULL);

  incident_controller_refresh_page(controller);

  save_incidents(queue, list, "data/incidents.bin");
}

void incident_controller_apply_filters(incident_controller_t *controller, int status, int priority)
{
  controller->status_filter = status;
  controller->priority_filter = priority;

  controller->pagination.page = 0;

  incident_controller_update_table(controller);
}

void incident_controller_search(incident_controller_t *controller, const char *text)
{
  if (text == NULL || strlen(text) == 0) return;

  char buffer[strlen(text) + 1];
  convert_to_uppercase(text, buffer);

  // TODO: Search by Technician Name and Equipment/Sensor ID
  switch (detect_search_type(buffer)) 
  {
    case SEARCH_INCIDENT_ID:
      snprintf(controller->search_text, ID_MAX, "%s", buffer);
      break;
    default:
      controller->search_text[0] = '\0';
      break;
  }

  controller->pagination.page = 0;

  incident_controller_update_table(controller);
}

void incident_controller_handle_toggled(incident_controller_t *controller, int id, bool is_active)
{
  incident_queue_t *queue = &controller->app->incidents_pending;
  incident_list_t *list = &controller->app->incidents_history;

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
    position = controller->pagination.page * 6 + row;
    
  return position;
}

void incident_controller_get_stats(incident_controller_t *controller, incident_stats_t *stats)
{
  stats->total = incident_get_count(&controller->app->incidents_pending, &controller->app->incidents_history);
  stats->pending = incident_queue_get_count(&controller->app->incidents_pending);
  stats->in_progress = incident_list_get_number_status(&controller->app->incidents_history, INCIDENT_IN_PROGRESS);
  stats->concluded = incident_list_get_number_status(&controller->app->incidents_history, INCIDENT_CONCLUDED);
}

gboolean on_incident_finished(gpointer data)
{
  incident_task_t *task = (incident_task_t *)data;

  task->controller->pagination.total = pagination_total_pages(task->controller->pagination, task->count);

  if (task->controller->pagination.page >= task->controller->pagination.total)
    task->controller->pagination.page = task->controller->pagination.total;

  incident_view_update_table(task->controller->view, task->result, task->count);

  free(task->params);
  free(task->result);
  free(task);

  return false;
}
