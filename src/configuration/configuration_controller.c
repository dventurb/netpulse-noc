#include "configuration_controller.h"

#include "configuration_worker.h"
#include "configuration_view.h"

#include "persistence.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void configuration_controller_init(configuration_controller_t *controller, configuration_view_t *view, void *data)
{
  controller->view = view;
  controller->app = (application_t *)data;

  controller->selected_equipment = NULL;

  controller->search_text[0] = '\0';

  controller->pagination.page = 0;
  controller->pagination.page_size = 6;
  //controller->pagination.total = configuration_get_count(&controller->app->equipments.configs);
}

void configuration_controller_execute_equipment_query(configuration_controller_t *controller, configuration_task_t *task)
{
  equipment_node_t *node = NULL;

  switch (detect_search_type(task->search_text)) 
  {
    case SEARCH_EQUIPMENT_ID:
      node = (equipment_node_t *) hashmap_get(&controller->app->id_index, task->search_text);
      break;
    case SEARCH_IP:
      node = (equipment_node_t *) hashmap_get(&controller->app->ip_index, task->search_text);
      break;
    case SEARCH_MAC:
      node = (equipment_node_t *) hashmap_get(&controller->app->mac_index, task->search_text);
      break;
    default:
      break;
  }
  
  if (node != NULL)
  {
    equipment_t *temp = malloc(sizeof(equipment_t));
    *temp = node->data;

    task->result = temp;
    task->count = 1;
    task->total = 1;
  }

  else 
  {
    equipment_list_t *list = &controller->app->equipments;
    task->total = equipment_get_count(list);
    task->result = equipment_list_in_range(list, task->start, task->end, &task->count);
  }

  g_idle_add(on_configuration_finish, task);
}

void configuration_controller_reset_equipment_query(configuration_controller_t *controller)
{
  controller->search_text[0] = '\0';

  configuration_worker_start_equipment_query(controller); // Create new thread so the UI doesnt freeze
}

void configuration_controller_start_equipment_query(configuration_controller_t *controller)
{
  configuration_worker_start_equipment_query(controller); // Create new thread so the UI doesnt freeze
}

bool configuration_controller_has_selected_equipment(configuration_controller_t *controller)
{
  if (controller->selected_equipment == NULL) return false;
  else return true;
}

void configuration_controller_add(configuration_controller_t *controller, configuration_t new)
{
  if (!configuration_controller_has_selected_equipment(controller)) return;

  configuration_stack_t *stack = &controller->selected_equipment->data.configs;
  equipment_list_t *list = &controller->app->equipments;

  configuration_stack_push(stack, new);

  //configuration_controller_start_query(controller);

  save_equipments(list, "data/equipments.bin");
}

void configuration_controller_set_search(configuration_controller_t *controller, const char *text)
{
  if (text == NULL) return;

  char buffer[strlen(text) + 1];
  convert_to_uppercase(text, buffer);

  switch (detect_search_type(buffer)) 
  {
    case SEARCH_EQUIPMENT_ID:
    case SEARCH_IP:
    case SEARCH_MAC:
      snprintf(controller->search_text, STRING_MAX, "%s", buffer);
      break;
    default:
      controller->search_text[0] = '\0';
      break;
  }

  configuration_controller_start_equipment_query(controller);
}

gboolean on_configuration_finish(gpointer data)
{
  configuration_task_t *task = (configuration_task_t *)data;

  if (task->type == TASK_EQUIPMENT)
    configuration_view_update_equipment_list(task->controller->view, task->result, task->count);

  else 
  {
    
  }

  free(task->result);
  free(task);

  return false;
}
