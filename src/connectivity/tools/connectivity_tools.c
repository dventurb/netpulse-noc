#include "connectivity_tools.h"

#include "ping_tool.h"
#include "traceroute_tool.h"
#include "dns_lookup_tool.h"
#include "arp_tool.h"

#include "app_data.h"

#include <stdlib.h>

struct connectivity_tool_manager_t {

  app_data_t *data;

  GtkStack *stack;

  connectivity_tool_t *tools[CONNECTIVITY_TOOL_COUNT];
  connectivity_tool_number_t current_tool;

};


GtkWidget *connectivity_tool_get_root(connectivity_tool_t *tool)
{
  if (tool) return tool->root;
  return NULL;
}

void connectivity_tool_refresh(connectivity_tool_t *tool)
{
  if (tool && tool->refresh)
    tool->refresh(tool);
}

void connectivity_tool_destroy(connectivity_tool_t *tool)
{
  if (tool && tool->destroy)
    tool->destroy(tool);
}

const connectivity_tool_descriptor_t *connectivity_tool_get_descriptor(connectivity_tool_number_t number)
{
  switch (number) 
  {
    case CONNECTIVITY_TOOL_PING:        return ping_tool_get_descriptor();
    case CONNECTIVITY_TOOL_TRACEROUTE:  return traceroute_tool_get_descriptor();
    case CONNECTIVITY_TOOL_DNS_LOOKUP:  return dns_lookup_tool_get_descriptor();
    case CONNECTIVITY_TOOL_ARP:         return arp_tool_get_descriptor();
    default:                            return NULL; 
  }
}

connectivity_tool_manager_t *connectivity_tool_manager_create(GtkStack *stack, app_data_t *data)
{
  connectivity_tool_manager_t *manager = malloc(sizeof(connectivity_tool_manager_t));
  if (manager == NULL) return NULL;

  manager->stack = stack;
  manager->data = data;
  manager->current_tool = CONNECTIVITY_TOOL_PING;

  for (int i = 0; i < CONNECTIVITY_TOOL_COUNT; i++)
    manager->tools[i] = NULL;

  return manager;
}

void connectivity_tool_manager_destroy(connectivity_tool_manager_t *manager)
{
  if (manager == NULL) return;

  for (int i = 0; i < CONNECTIVITY_TOOL_COUNT; i++) 
    if (manager->tools[i]) connectivity_tool_destroy(manager->tools[i]);

  free(manager);
}

bool connectivity_tool_manager_show(connectivity_tool_manager_t *manager, connectivity_tool_number_t number)
{
  if (manager == NULL) return false;
  
  if (number != CONNECTIVITY_TOOL_PING) return false; // TODO: implementing others tools

  const connectivity_tool_descriptor_t *descriptor = connectivity_tool_get_descriptor(number);
  if (descriptor == NULL) return false;

  if (manager->tools[number] == NULL)
  {
    connectivity_tool_t *tool = descriptor->create(manager->data);
    if (tool == NULL) return false;

    manager->tools[number] = tool;

    gtk_stack_add_named(manager->stack, connectivity_tool_get_root(tool), descriptor->stack_name);
  }

  gtk_stack_set_visible_child_name(manager->stack, descriptor->stack_name);

  connectivity_tool_refresh(manager->tools[number]);
  manager->current_tool = number;

  return true;
}

connectivity_tool_number_t connectivity_tool_manager_get_current(connectivity_tool_manager_t *manager)
{
  return manager->current_tool;
}
