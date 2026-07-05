#include "connectivity_view.h"

static void build_layout(connectivity_view_t *view);
static void setup_tools(connectivity_view_t *view);

static GtkWidget *build_header(connectivity_view_t *view);
static GtkWidget *build_sidebar(connectivity_view_t *view);
static GtkWidget *build_topbar(connectivity_view_t *view);

static void sync_sidebar_icons(connectivity_view_t *view, int index);

// Callbacks
static void on_tool_selected(int index, void *data);


GtkBox *connectivity_view_create(connectivity_view_t *view, connectivity_controller_t *controller)
{
  view->controller = controller;

  build_layout(view);
  setup_tools(view);

  on_tool_selected(CONNECTIVITY_TOOL_PING, view);

  return view->container;
}

void connectivity_view_destroy(connectivity_view_t *view)
{
  tab_bar_destroy(&view->sidebar_tools);
  tab_bar_destroy(&view->topbar_tools);

  if (view->tool_manager != NULL)
  {
    connectivity_tool_manager_destroy(view->tool_manager);
    free(view->tool_manager);
  }
}

static void build_layout(connectivity_view_t *view)
{
  view->container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

  GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  
  GtkWidget *header = build_header(view);
  view->stack = GTK_STACK(gtk_stack_new());

  GtkWidget *sidebar = build_sidebar(view);

  gtk_box_append(GTK_BOX(right_box), header);
  gtk_box_append(GTK_BOX(right_box), GTK_WIDGET(view->stack));

  gtk_box_append(view->container, sidebar);
  gtk_box_append(view->container, right_box);
}

static void setup_tools(connectivity_view_t *view)
{
  view->tool_manager = connectivity_tool_manager_create(view->stack, view->controller->data);

  connectivity_tool_manager_show(view->tool_manager, CONNECTIVITY_TOOL_PING);
  sync_sidebar_icons(view, CONNECTIVITY_TOOL_PING);
}

static GtkWidget *build_header(connectivity_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_size_request(container, -1, 112);
  gtk_widget_add_css_class(container, "connectivity-header");

  GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_set_margin_top(title_box, 24);
  gtk_widget_set_margin_start(title_box, 32);

  GtkWidget *logo = gtk_image_new_from_file("assets/icon-connectivity.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(logo), 32);
  
  GtkWidget *title = gtk_label_new("Connectivity Tools");
  gtk_widget_add_css_class(title, "connectivity-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  gtk_box_append(GTK_BOX(title_box), logo);
  gtk_box_append(GTK_BOX(title_box), title);

  GtkWidget *topbar = build_topbar(view);
  gtk_widget_set_margin_start(topbar, 32);

  gtk_box_append(GTK_BOX(container), title_box);
  gtk_box_append(GTK_BOX(container), topbar);

  return container;
}

static GtkWidget *build_sidebar(connectivity_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(container, 256, -1);
  gtk_widget_add_css_class(container, "connectivity-sidebar");

  GtkWidget *label = gtk_label_new("NETWORK TOOLS");
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_widget_add_css_class(label, "connectivity-sidebar-label");

  view->sidebar_tools = tab_bar_new(CONNECTIVITY_TOOL_COUNT, GTK_ORIENTATION_VERTICAL, on_tool_selected, view);

  const char *labels[CONNECTIVITY_TOOL_COUNT];
  const char *icons[CONNECTIVITY_TOOL_COUNT];

  for (int i = 0; i < CONNECTIVITY_TOOL_COUNT; i++) 
  {
    const connectivity_tool_descriptor_t *descriptor = connectivity_tool_get_descriptor(i);
    labels[i] = descriptor->text;
    icons[i] = descriptor->icon_default;
  }

  tab_bar_create_buttons(&view->sidebar_tools, labels, icons, "connectivity-sidebar-button");

  gtk_widget_set_vexpand(GTK_WIDGET(view->sidebar_tools.container), TRUE);
  gtk_widget_add_css_class(GTK_WIDGET(view->sidebar_tools.container), "connectivity-sidebar");

  gtk_box_append(GTK_BOX(container), label);
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->sidebar_tools.container));

  return container;
}

static GtkWidget *build_topbar(connectivity_view_t *view)
{
  view->topbar_tools = tab_bar_new(CONNECTIVITY_TOOL_COUNT, GTK_ORIENTATION_HORIZONTAL, on_tool_selected, view);

  const char *labels[CONNECTIVITY_TOOL_COUNT];

  for (int i = 0; i < CONNECTIVITY_TOOL_COUNT; i++) 
  {
    const connectivity_tool_descriptor_t *descriptor = connectivity_tool_get_descriptor(i);
    labels[i] = descriptor->text;
  }

  tab_bar_create_buttons(&view->topbar_tools, labels, NULL, "connectivity-tools-button");

  gtk_widget_set_hexpand(GTK_WIDGET(view->topbar_tools.container), TRUE);

  return GTK_WIDGET(view->topbar_tools.container);
}

static void sync_sidebar_icons(connectivity_view_t *view, int index)
{
  for (int i = 0; i < CONNECTIVITY_TOOL_COUNT; i++) 
  {
    const connectivity_tool_descriptor_t *descriptor = connectivity_tool_get_descriptor(i);

    const char *icon = i == index ? descriptor->icon_active : descriptor->icon_default;

    toggle_button_set_icon(&view->sidebar_tools.buttons[i], icon);
  }
}

static void on_tool_selected(int index, void *data)
{
  if (index < 0 || index >= CONNECTIVITY_TOOL_COUNT) return;

  connectivity_view_t *view = (connectivity_view_t *)data;

  if (connectivity_tool_manager_show(view->tool_manager, index) == FALSE)
  {
    connectivity_tool_number_t previous = connectivity_tool_manager_get_current(view->tool_manager);

    tab_bar_set_selected(&view->sidebar_tools, previous);
    tab_bar_set_selected(&view->topbar_tools, previous);

    sync_sidebar_icons(view, previous);
  }
}
