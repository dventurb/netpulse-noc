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
  controller->pagination.total = 0; // blank table
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
  controller->pagination.page = 0;
  controller->pagination.total = 0;

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
  equipment_list_t *list = &controller->app->equipments;

  configuration_stack_push(stack, new); // O(1) - There is no problem insert in the main thread

  save_equipments(list, "data/equipments.bin"); // TODO: But I need a new thread for this one

  configuration_controller_start_config_query(controller);
}

void configuration_controller_execute_clear(configuration_controller_t *controller, configuration_task_t *task)
{
  configuration_stack_t *stack = &controller->selected_equipment->data.configs;
  equipment_list_t *list = &controller->app->equipments;

  configuration_stack_destroy(stack);
  configuration_stack_init(stack);

  save_equipments(list, "data/equipments.bin");

  configuration_controller_execute_config_query(controller, task);
}

void configuration_controller_start_clear(configuration_controller_t *controller)
{
  if (!configuration_controller_has_selected_equipment(controller)) return;

  configuration_stack_t *stack = &controller->selected_equipment->data.configs;

  if (stack->top == NULL) return;

  configuration_worker_clear(controller);
}

// This is just a revert animation, I have to copy the configs data to a temp array, destroy the nodes in the orginal stack, and then push them again the configs data from total->count - 1 to 0.
void configuration_controller_execute_revert(configuration_controller_t *controller, configuration_task_t *task)
{
  configuration_stack_t *stack = &controller->selected_equipment->data.configs;

  if (stack->top != NULL) configuration_stack_pop(stack);

  task->count = 0;
  task->result = configuration_stack_in_range(stack, 0, stack->count, &task->total);

  configuration_stack_destroy(stack); // destroy all config nodes
  configuration_stack_init(stack);  // rebuild again

  task->controller->pagination.total = task->total;

  g_timeout_add(250, on_configuration_revert, task); // then repush the old data
}

void configuration_controller_start_revert(configuration_controller_t *controller)
{
  if (!configuration_controller_has_selected_equipment(controller)) return;

  configuration_stack_t *stack = &controller->selected_equipment->data.configs;

  if (stack->top == NULL) return;

  controller->pagination.page = 0;

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

  hashmap_t *id_index = &controller->app->id_index;

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
}

gboolean on_configuration_finish(gpointer data)
{
  configuration_task_t *task = (configuration_task_t *)data;

  if (task->type == TASK_EQUIPMENT)
    configuration_view_update_equipment_list(task->controller->view, task->result, task->count);

  else if (task->type == TASK_CONFIGURATION)
  {
    task->controller->pagination.total = task->total;

    int total_pages = pagination_total_pages(task->controller->pagination, task->total);

    if (task->controller->pagination.page >= total_pages - 1)
      task->controller->pagination.page = total_pages - 1;

    if (task->controller->pagination.page < 0)
      task->controller->pagination.page = 0;

    configuration_view_update_config_table(task->controller->view, task->result, task->count);
  }

  free(task->result);
  free(task);

  return false;
}

gboolean on_configuration_revert(gpointer data)
{
  configuration_task_t *task = (configuration_task_t *)data;

  configuration_stack_t *stack = &task->controller->selected_equipment->data.configs;
  equipment_list_t *list = &task->controller->app->equipments;

  configuration_t *configs = (configuration_t *)task->result;

  if (task->total == 0)
    configuration_view_update_config_table(task->controller->view, NULL, 0); // blank table
  if (task->count++ < task->total)
  {
    int i = task->total - task->count;
    configuration_stack_repush(stack, configs[i]);

    int start = 0;
    if (task->count > 6)
      start = task->count - 6;

    printf("%s\n", configs[i].command);
    printf("count: %d | total: %d | start: %d | i: %d\n\n", task->count, task->total, start, i);

    configuration_view_update_config_table(task->controller->view, &configs[i], (task->count - start));

    return true; // continue until task->count == task->total
  }

  save_equipments(list, "data/equipments.bin");

  free(task->result);
  free(task);

  return false;
}
