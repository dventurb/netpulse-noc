#include "dns_lookup_tool.h"

// TODO: #include "dns_lookup_view.h"
// TODO: #include "dns_lookup_controller.h"

typedef struct {

  connectivity_tool_t parent;

  // TODO: dns_lookup_view_t        view;
  // TODO: dns_lookup_controller_t  controller;

} dns_lookup_tool_t;


static connectivity_tool_t *dns_lookup_tool_create(app_data_t *data);


static const connectivity_tool_descriptor_t descriptor = {
  
  .number = CONNECTIVITY_TOOL_DNS_LOOKUP,
  
  .text   = "DNS Lookup",
  .stack_name = "DNS_LOOKUP",

  .icon_default = "assets/icon-dns-lookup-default.svg",
  .icon_active = "assets/icon-dns-lookup-active.svg",

  .create = dns_lookup_tool_create,

};


static void dns_lookup_tool_destroy(connectivity_tool_t *parent)
{
  dns_lookup_tool_t *tool = (dns_lookup_tool_t *)parent;

  free(tool);
}

static void dns_lookup_tool_refresh(connectivity_tool_t *parent)
{
  (void)parent;
  //dns_lookup_tool_t *tool = (dns_lookup_tool_t *)parent;

  //dns_lookup_view_refresh(&tool->view);
}

static connectivity_tool_t *dns_lookup_tool_create(app_data_t *data)
{
  (void)data;
  return NULL;
  
  /*
  dns_lookup_tool_t *tool = malloc(sizeof(dns_lookup_tool_t));
  if (tool == NULL) return NULL;

  dns_lookup_controller_init(&tool->controller, &tool->view, data);

  tool->parent.root = GTK_WIDGET(dns_lookup_view_create(&tool->view, &tool->controller));

  tool->parent.refresh = dns_lookup_tool_refresh;
  tool->parent.destroy = dns_lookup_tool_destroy;

  return &tool->parent;
  */
}

const connectivity_tool_descriptor_t *dns_lookup_tool_get_descriptor(void)
{
  return &descriptor;
}
