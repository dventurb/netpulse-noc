#include "arp_tool.h"

// TODO: #include "arp_view.h"
// TODO: #include "arp_controller.h"

typedef struct {

  connectivity_tool_t parent;

  // TODO: arp_view_t        view;
  // TODO: arp_controller_t  controller;

} arp_tool_t;


static connectivity_tool_t *arp_tool_create(app_data_t *data);


static const connectivity_tool_descriptor_t descriptor = {
  
  .number = CONNECTIVITY_TOOL_ARP,
  
  .text   = "ARP",
  .stack_name = "ARP",

  .icon_default = "assets/icon-arp-table-default.svg",
  .icon_active = "assets/icon-arp-table-active.svg",

  .create = arp_tool_create,

};


static void arp_tool_destroy(connectivity_tool_t *parent)
{
  arp_tool_t *tool = (arp_tool_t *)parent;

  free(tool);
}

static void arp_tool_refresh(connectivity_tool_t *parent)
{
  (void)parent;
  //arp_tool_t *tool = (arp_tool_t *)parent;

  //arp_view_refresh(&tool->view);
}

static connectivity_tool_t *arp_tool_create(app_data_t *data)
{
  (void)data;
  return NULL;
  
  /*
  arp_tool_t *tool = malloc(sizeof(arp_tool_t));
  if (tool == NULL) return NULL;

  arp_controller_init(&tool->controller, &tool->view, data);

  tool->parent.root = GTK_WIDGET(arp_view_create(&tool->view, &tool->controller));

  tool->parent.refresh = arp_tool_refresh;
  tool->parent.destroy = arp_tool_destroy;

  return &tool->parent;
  */
}

const connectivity_tool_descriptor_t *arp_tool_get_descriptor(void)
{
  return &descriptor;
}
