#ifndef CONNECTIVITY_TOOLS_H
#define CONNECTIVITY_TOOLS_H

#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct app_data_t app_data_t;

typedef struct connectivity_tool_manager_t connectivity_tool_manager_t; // opaque struct

typedef enum {

  CONNECTIVITY_TOOL_PING,
  CONNECTIVITY_TOOL_TRACEROUTE,
  CONNECTIVITY_TOOL_DNS_LOOKUP,
  CONNECTIVITY_TOOL_ARP,

  CONNECTIVITY_TOOL_COUNT

} connectivity_tool_number_t;

typedef struct connectivity_tool_t {

  GtkWidget *root;

  void (*refresh)(struct connectivity_tool_t *tool);
  void (*destroy)(struct connectivity_tool_t *tool);

} connectivity_tool_t;

typedef struct {

  connectivity_tool_number_t number;

  const char *text;
  const char *stack_name;

  const char *icon_default;
  const char *icon_active;

  connectivity_tool_t *(*create)(app_data_t *);

} connectivity_tool_descriptor_t;


GtkWidget *connectivity_tool_get_root(connectivity_tool_t *tool);
void connectivity_tool_refresh(connectivity_tool_t *tool);
void connectivity_tool_destroy(connectivity_tool_t *tool);

connectivity_tool_manager_t *connectivity_tool_manager_create(GtkStack *stack, app_data_t *data);
void connectivity_tool_manager_destroy(connectivity_tool_manager_t *manager);
bool connectivity_tool_manager_show(connectivity_tool_manager_t *manager, connectivity_tool_number_t number);
connectivity_tool_number_t connectivity_tool_manager_get_current(connectivity_tool_manager_t *manager);

const connectivity_tool_descriptor_t *connectivity_tool_get_descriptor(connectivity_tool_number_t number);

#endif
