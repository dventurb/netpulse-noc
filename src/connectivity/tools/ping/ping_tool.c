#include "ping_tool.h"

#include "ping_view.h"
#include "ping_controller.h"

typedef struct {

  connectivity_tool_t parent;

  ping_view_t        view;
  ping_controller_t  controller;

} ping_tool_t;


static connectivity_tool_t *ping_tool_create(app_data_t *data);


static const connectivity_tool_descriptor_t descriptor = {
  
  .number = CONNECTIVITY_TOOL_PING,
  
  .text   = "Ping",
  .stack_name = "PING",

  .icon_default = "assets/icon-ping-default.svg",
  .icon_active = "assets/icon-ping-active.svg",

  .create = ping_tool_create,

};


static void ping_tool_destroy(connectivity_tool_t *parent)
{
  ping_tool_t *tool = (ping_tool_t *)parent;

  free(tool);
}

static void ping_tool_refresh(connectivity_tool_t *parent)
{
  (void)parent;
  //ping_tool_t *tool = (ping_tool_t *)parent;

  //ping_view_refresh(&tool->view);
}

static connectivity_tool_t *ping_tool_create(app_data_t *data)
{
  ping_tool_t *tool = malloc(sizeof(ping_tool_t));
  if (tool == NULL) return NULL;

  ping_controller_init(&tool->controller, &tool->view, data);

  tool->parent.root = GTK_WIDGET(ping_view_create(&tool->view, &tool->controller));

  tool->parent.refresh = ping_tool_refresh;
  tool->parent.destroy = ping_tool_destroy;

  return &tool->parent;
}

const connectivity_tool_descriptor_t *ping_tool_get_descriptor(void)
{
  return &descriptor;
}
