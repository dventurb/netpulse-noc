#include "traceroute_tool.h"

// TODO: #include "traceroute_view.h"
// TODO: #include "traceroute_controller.h"

typedef struct {

  connectivity_tool_t parent;

  // TODO: traceroute_view_t        view;
  // TODO: traceroute_controller_t  controller;

} traceroute_tool_t;


static connectivity_tool_t *traceroute_tool_create(app_data_t *data);


static const connectivity_tool_descriptor_t descriptor = {
  
  .number = CONNECTIVITY_TOOL_TRACEROUTE,
  
  .text   = "TraceRoute",
  .stack_name = "TRACEROUTE",

  .icon_default = "assets/icon-traceroute-default.svg",
  .icon_active = "assets/icon-traceroute-active.svg",

  .create = traceroute_tool_create,

};


static void traceroute_tool_destroy(connectivity_tool_t *parent)
{
  (void)parent;
  traceroute_tool_t *tool = (traceroute_tool_t *)parent;

  free(tool);
}

static void traceroute_tool_refresh(connectivity_tool_t *parent)
{
  (void)parent;
  //traceroute_tool_t *tool = (traceroute_tool_t *)parent;

  //traceroute_view_refresh(&tool->view);
}

static connectivity_tool_t *traceroute_tool_create(app_data_t *data)
{
  (void)data;
  return NULL;
  
  /*
  traceroute_tool_t *tool = malloc(sizeof(traceroute_tool_t));
  if (tool == NULL) return NULL;

  traceroute_controller_init(&tool->controller, &tool->view, data);

  tool->parent.root = GTK_WIDGET(traceroute_view_create(&tool->view, &tool->controller));

  tool->parent.refresh = traceroute_tool_refresh;
  tool->parent.destroy = traceroute_tool_destroy;

  return &tool->parent;
  */
}

const connectivity_tool_descriptor_t *traceroute_tool_get_descriptor(void)
{
  return &descriptor;
}
