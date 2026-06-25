#include "configuration_controller.h"

#include "configuration_worker.h"
#include "configuration_view.h"

#include "persistence.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void configuration_controller_init(configuration_controller_t *controller, configuration_view_t *view, app_data_t *data)
{
  controller->view = view;
  controller->data = data;

  controller->selected_equipment = NULL;

  controller->search_text[0] = '\0';

  pagination_init(&controller->pagination, 0); // no selected equipment
}

void configuration_controller_execute_equipment_query(configuration_controller_t *controller, configuration_task_t *task)
{
  equipment_node_t *node = NULL;

  switch (detect_search_type(task->search_text)) 
  {
    case SEARCH_EQUIPMENT_ID:
      node = (equipment_node_t *) hashmap_get(&controller->data->id_index, task->search_text);
      break;
    case SEARCH_IP:
      node = (equipment_node_t *) hashmap_get(&controller->data->ip_index, task->search_text);
      break;
    case SEARCH_MAC:
      node = (equipment_node_t *) hashmap_get(&controller->data->mac_index, task->search_text);
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
    equipment_list_t *list = &controller->data->equipments;
    task->total = equipment_get_count(list);
    task->result = equipment_list_in_range(list, task->start, task->end, &task->count);
  }

  g_idle_add(on_configuration_finish, task);
}

void configuration_controller_reset_equipment_query(configuration_controller_t *controller)
{
  controller->selected_equipment = NULL;
  controller->search_text[0] = '\0';

  configuration_worker_start_equipment_query(controller); // Create new thread so the UI doesnt freeze
}

void configuration_controller_start_equipment_query(configuration_controller_t *controller)
{
  configuration_worker_start_equipment_query(controller); // Create new thread so the UI doesnt freeze
}

static void configuration_controller_execute_pagination(configuration_task_t *task, configuration_stack_t *stack)
{
  task->total = configuration_get_count(stack);
  task->result = configuration_stack_in_range(stack, task->start, task->end, &task->count);
}

void configuration_controller_execute_config_query(configuration_controller_t *controller, configuration_task_t *task)
{
  configuration_stack_t *stack = &controller->selected_equipment->data.configs;

  configuration_controller_execute_pagination(task, stack);

  g_idle_add(on_configuration_finish, task);
}

void configuration_controller_reset_config_query(configuration_controller_t *controller)
{
  pagination_init(&controller->pagination, 0); // no selected equipment
  configuration_view_update_config_table(controller->view, NULL, 0); // blank table
}

void configuration_controller_start_config_query(configuration_controller_t *controller)
{
  if (!configuration_controller_has_selected_equipment(controller)) return;
  else configuration_worker_start_config_query(controller); // Create new thread
}

void configuration_controller_add(configuration_controller_t *controller, configuration_t new)
{
  if (!configuration_controller_has_selected_equipment(controller)) return;

  configuration_stack_t *stack = &controller->selected_equipment->data.configs;
  equipment_list_t *list = &controller->data->equipments;

  configuration_stack_push(stack, new); // O(1) - There is no problem insert in the main thread

  save_equipments(list); // TODO: But I need a new thread for this one

  configuration_controller_start_config_query(controller);
}

void configuration_controller_execute_clear(configuration_controller_t *controller, configuration_task_t *task)
{
  configuration_stack_t *stack = &controller->selected_equipment->data.configs;
  equipment_list_t *list = &controller->data->equipments;

  configuration_stack_destroy(stack);
  configuration_stack_init(stack);

  save_equipments(list);

  configuration_controller_execute_config_query(controller, task);
}

void configuration_controller_start_clear(configuration_controller_t *controller)
{
  if (!configuration_controller_has_selected_equipment(controller)) return;

  configuration_stack_t *stack = &controller->selected_equipment->data.configs;

  if (stack->top == NULL) return;

  configuration_worker_clear(controller);
}

void configuration_controller_execute_revert(configuration_controller_t *controller, configuration_task_t *task)
{
  configuration_stack_t *stack = &controller->selected_equipment->data.configs;
  equipment_list_t *list = &controller->data->equipments;

  if (stack->top != NULL) configuration_stack_pop(stack);

  task->count = 0;
  task->result = configuration_stack_in_range(stack, 0, stack->count, &task->total);

  configuration_stack_destroy(stack);
  configuration_stack_init(stack);
  
  configuration_t *configs = (configuration_t *)task->result;
  
  // then repush the old data 
  for (int i = task->total - 1; i >= 0; i--)
    configuration_stack_repush(stack, configs[i]);

  task->controller->pagination.total_items = task->total;

  save_equipments(list);

  int timeout = 2500 / (task->total ? task->total : 1);
  if (timeout > 250) timeout = 250;
  if (timeout < 16) timeout = 16; 

  g_timeout_add(timeout, on_configuration_revert, task); // fake animation with timeout
}

void configuration_controller_start_revert(configuration_controller_t *controller)
{
  if (!configuration_controller_has_selected_equipment(controller)) return;

  configuration_stack_t *stack = &controller->selected_equipment->data.configs;

  if (stack->top == NULL) return;

  controller->pagination.current_page = 0;

  configuration_worker_revert(controller);
}

void configuration_controller_set_search(configuration_controller_t *controller, const char *text)
{
  if (text == NULL) return;

  char buffer[strlen(text) + 1];
  convert_to_uppercase(text, buffer);

  switch (detect_search_type(buffer)) 
  {
    case SEARCH_EQUIPMENT_ID: case SEARCH_IP: case SEARCH_MAC:
      snprintf(controller->search_text, STRING_MAX, "%s", buffer);
      break;
    default:
      controller->search_text[0] = '\0';
      break;
  }

  configuration_controller_start_equipment_query(controller);
}

void configuration_controller_set_selected_equipment(configuration_controller_t *controller, const char *id)
{
  if (id == NULL) return;

  hashmap_t *id_index = &controller->data->id_index;

  equipment_node_t *node = (equipment_node_t *)hashmap_get(id_index, id);
  if (node == NULL) return;

  controller->selected_equipment = node;
}

bool configuration_controller_has_selected_equipment(configuration_controller_t *controller)
{
  if (controller->selected_equipment == NULL) return false;
  else return true;
}

equipment_t *configuration_controller_get_selected_equipment(configuration_controller_t *controller)
{
  if (!configuration_controller_has_selected_equipment(controller)) return NULL;
  return &controller->selected_equipment->data;
}

bool configuration_controller_validate(configuration_t new)
{
  if (strlen(new.command) <= 1 || strlen(new.command) >= COMMAND_MAX) return false;
  else return true;
}

bool configuration_controller_is_top_stack(configuration_controller_t *controller, int number)
{
  if (!configuration_controller_has_selected_equipment(controller)) return false;
  if(controller->selected_equipment->data.configs.top == NULL) return false;
  if (number == controller->selected_equipment->data.configs.top->data.number) return true;
  return false;
}

void configuration_controller_get_stats(configuration_controller_t *controller, configuration_stats_t *stats)
{
  if (!configuration_controller_has_selected_equipment(controller)) return;
  
  stats->total = configuration_get_count(&controller->selected_equipment->data.configs);
  
  if (controller->selected_equipment->data.configs.count != 0)
    format_timestamp_to_datetime(controller->selected_equipment->data.configs.top->data.operated_at, stats->last_updated);
}

gboolean on_configuration_finish(gpointer data)
{
  configuration_task_t *task = (configuration_task_t *)data;

  if (task->type == TASK_EQUIPMENT)
    configuration_view_update_equipment_list(task->controller->view, task->result, task->count);

  else if (task->type == TASK_CONFIGURATION)
  {
    task->controller->pagination.total_items = task->total;

    int total_pages = pagination_total_pages(task->controller->pagination, task->total);

    pagination_fix_current_page(&task->controller->pagination, total_pages);

    configuration_view_update_config_table(task->controller->view, task->result, task->count);
    configuration_view_update_cards(task->controller->view);
  }

  free(task->result);
  free(task);

  return false;
}

/*
 *
|    Total    |  items_per_iter  |  Steps  |  Timeout  |  Real Time |
| ----------- | ---------------- | ------- | --------- | ---------- |
|   1         |       1          |   1     |     250   |   0,25 sec |
|   10        |       1          |   10    |     250   |   2,5 sec  |
|   100       |       1          |   100   |     25    |   2,5 sec  |
|   1000      |       10         |   100   |     16    |   1,6 sec  |
|   10000     |       100        |   100   |     16    |   1,6 sec  |
|   100000    |       1000       |   100   |     16    |   1,6 sec  |
|   1000000   |       10000      |   100   |     16    |   1,6 sec  |
*
*/
gboolean on_configuration_revert(gpointer data)
{
  configuration_task_t *task = (configuration_task_t *)data;
  configuration_t *configs = (configuration_t *)task->result;
  
  int page_size = task->controller->pagination.page_size;

  int items_per_iter = task->total / 100;
  if (items_per_iter < 1) items_per_iter = 1;

  if (task->total == 0)
    configuration_view_update_config_table(task->controller->view, NULL, 0); // blank table
  if (task->count < task->total)
  {
    task->count += items_per_iter;
    if (task->count > task->total) task->count = task->total;

    int i = task->total - task->count;

    int start = 0;
    if (task->count > page_size) start = task->count - page_size;

    configuration_view_update_config_table(task->controller->view, &configs[i], (task->count - start));

    return true; // continue until task->count == task->total
  }

  free(task->result);
  free(task);

  return false;
}
