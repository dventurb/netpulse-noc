#include "ui_connectivity.h"

#include "utils.h"
#include "ui_widgets.h"
#include "macros.h"
#include "ui_macros.h"

static GtkWidget *create_connectivity_side_bar(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_connectivity_header(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_menu_bar(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_page_ping(ui_connectivity_t *ui_connectivity);

static GtkWidget *create_ping_header(void);
static GtkWidget *create_target_source_selector(GtkWidget *stack);
static GtkWidget *create_equipment_search(ui_connectivity_t *ui_connectivity);
static void create_ping_parameters_section(GtkWidget *grid);
static void create_ping_actions(GtkWidget *grid);
static GtkWidget *create_connectivity_terminal(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_equipment_container(equipment_node_t *node);

static void synchronize_navigation(ui_connectivity_t *ui_connectivity, GtkWidget *button);

// Callbacks
static void on_menu_button_clicked(GtkButton *button, gpointer data);
static void on_sidebar_button_clicked(GtkButton *button, gpointer data);
static void on_search_equipment_activated(GtkSearchEntry *search, gpointer data);
static void on_target_source_selection_clicked(GtkButton *button, gpointer data);

GtkWidget *create_page_connectivity(ui_t *ui)
{
  ui_connectivity_t *ui_connectivity = malloc(sizeof(ui_connectivity_t));
  if (ui_connectivity == NULL) return NULL;

  ui_connectivity->application = ui->application;
  ui_connectivity->window = ui->window;

  ui_connectivity->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  ui_connectivity->stack = gtk_stack_new();

  GtkWidget *side_bar = create_connectivity_side_bar(ui_connectivity);

  GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  GtkWidget *header = create_connectivity_header(ui_connectivity);

  gtk_stack_add_named(GTK_STACK(ui_connectivity->stack), create_page_ping(ui_connectivity), "Ping");

  gtk_box_append(GTK_BOX(right_box), header);
  gtk_box_append(GTK_BOX(right_box), ui_connectivity->stack);

  gtk_box_append(GTK_BOX(ui_connectivity->container), side_bar);
  gtk_box_append(GTK_BOX(ui_connectivity->container), right_box);

  gtk_stack_set_visible_child_name(GTK_STACK(ui_connectivity->stack), "Ping");

  g_object_set_data_full(G_OBJECT(ui_connectivity->container), "ui_connectivity", ui_connectivity, free); // ownership + free

  return ui_connectivity->container;
}

static GtkWidget *create_connectivity_side_bar(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 256, -1);
  gtk_widget_set_vexpand(box, TRUE);
  gtk_widget_add_css_class(box, "connectivity-sidebar");

  GtkWidget *label = gtk_label_new("NETWORK TOOLS");
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_widget_add_css_class(label, "connectivity-sidebar-label");
  gtk_box_append(GTK_BOX(box), label);

  const char *tools[] = {
    "Ping",
    "TraceRoute",
    "DNS Lookup",
    "ARP"
  };

  const char *icons[] = {
    "assets/icon-ping-active.svg",
    "assets/icon-traceroute-default.svg",
    "assets/icon-dns-lookup-default.svg",
    "assets/icon-arp-table-default.svg"
  };

  for (int i = 0; i < 4; i++)
  {
    ui_connectivity->sidebar_buttons[i] = create_primary_button(tools[i], icons[i],"connectivity-sidebar-button"); 
    g_object_set_data(G_OBJECT(ui_connectivity->sidebar_buttons[i]), "target-page", (void *)tools[i]);
    g_signal_connect(ui_connectivity->sidebar_buttons[i], "clicked", G_CALLBACK(on_sidebar_button_clicked), ui_connectivity);

    gtk_box_append(GTK_BOX(box), ui_connectivity->sidebar_buttons[i]);
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui_connectivity->sidebar_buttons[0]), TRUE);

  return box;
}

static GtkWidget *create_connectivity_header(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_size_request(box, -1, 112);
  gtk_widget_add_css_class(box, "connectivity-header");

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

  GtkWidget *menu_bar = create_menu_bar(ui_connectivity);
  gtk_widget_set_margin_start(menu_bar, 32);

  gtk_box_append(GTK_BOX(box), title_box);
  gtk_box_append(GTK_BOX(box), menu_bar);

  return box;
}

static GtkWidget *create_menu_bar(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 24);

  const char *tools[] = {
    "Ping",
    "TraceRoute",
    "DNS Lookup",
    "ARP"
  };

  for (int i = 0; i < 4; i++)
  {
    ui_connectivity->buttons[i] = create_primary_button(tools[i], NULL, "connectivity-tools-button"); 
    g_object_set_data(G_OBJECT(ui_connectivity->buttons[i]), "target-page", (void *)tools[i]);
    g_signal_connect(ui_connectivity->buttons[i], "clicked", G_CALLBACK(on_menu_button_clicked), ui_connectivity);

    gtk_box_append(GTK_BOX(box), ui_connectivity->buttons[i]);
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui_connectivity->buttons[0]), TRUE);

  return box;
}

static GtkWidget *create_page_ping(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_margin_top(container, 32);
  gtk_widget_set_margin_start(container, 32);
  gtk_widget_set_margin_end(container, 32);

  GtkWidget *config_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_widget_set_size_request(config_panel, -1, 400);
  gtk_widget_add_css_class(config_panel, "connectivity-config-panel");

  GtkWidget *panel_header = create_ping_header();

  GtkWidget *label_target = gtk_label_new("TARGET SOURCE SELECTION");
  gtk_widget_set_halign(label_target, GTK_ALIGN_START);
  gtk_widget_add_css_class(label_target, "config-panel-form-label");

  GtkWidget *target_stack = gtk_stack_new();

  GtkWidget *target_source_selector = create_target_source_selector(target_stack);

  GtkWidget *form_fields_grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(form_fields_grid), 16);
  gtk_grid_set_column_spacing(GTK_GRID(form_fields_grid), 16);

  GtkWidget *container_search = create_equipment_search(ui_connectivity);
  gtk_stack_add_named(GTK_STACK(target_stack), container_search, "target-registered-equipment");
  gtk_stack_set_visible_child_name(GTK_STACK(target_stack), "target-registered-equipment");
 
  GtkWidget *manual_target_field = create_unit_field("TARGET (IP / HOSTNAME)", "192.168.1.1", NULL);
  gtk_stack_add_named(GTK_STACK(target_stack), manual_target_field, "target-manual-ip");

  gtk_grid_attach(GTK_GRID(form_fields_grid), target_stack, 0, 1, 2, 1);

  create_ping_parameters_section(form_fields_grid);
  create_ping_actions(form_fields_grid);

  gtk_box_append(GTK_BOX(config_panel), panel_header);
  gtk_box_append(GTK_BOX(config_panel), label_target);
  gtk_box_append(GTK_BOX(config_panel), target_source_selector);
  gtk_box_append(GTK_BOX(config_panel), form_fields_grid);

  GtkWidget *terminal_panel = create_connectivity_terminal(ui_connectivity);
  gtk_box_append(GTK_BOX(container), config_panel);
  gtk_box_append(GTK_BOX(container), terminal_panel);

  return container;
}

static GtkWidget *create_ping_header(void)
{
  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_add_css_class(header, "config_panel-header");
  gtk_widget_set_valign(header, GTK_ALIGN_START);
  
  GtkWidget *header_icon = gtk_image_new_from_file("assets/icon-ping-configuration.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(header_icon), 14);

  GtkWidget *header_title = gtk_label_new("Ping Configuration");
  gtk_widget_add_css_class(header_title, "config-panel-title");

  gtk_box_append(GTK_BOX(header), header_icon);
  gtk_box_append(GTK_BOX(header), header_title);

  return header;
}

static GtkWidget *create_target_source_selector(GtkWidget *stack)
{
  GtkWidget *target_source_selector = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);

  GtkWidget *radio_equipment = gtk_check_button_new_with_label("Registered equipment");
  gtk_widget_add_css_class(radio_equipment, "config-panel-form-checkbutton");
  g_object_set_data(G_OBJECT(radio_equipment), "target-source", "target-registered-equipment");
  gtk_check_button_set_active(GTK_CHECK_BUTTON(radio_equipment), TRUE);
  g_signal_connect(radio_equipment, "toggled", G_CALLBACK(on_target_source_selection_clicked), stack);

  GtkWidget *radio_manual_input = gtk_check_button_new_with_label("Manual IP / Hostname");
  gtk_widget_add_css_class(radio_manual_input, "config-panel-form-checkbutton");
  g_object_set_data(G_OBJECT(radio_manual_input), "target-source", "target-manual-ip");
  g_signal_connect(radio_manual_input, "toggled", G_CALLBACK(on_target_source_selection_clicked), stack);

  gtk_check_button_set_group(GTK_CHECK_BUTTON(radio_equipment), GTK_CHECK_BUTTON(radio_manual_input)); // mutually exclusive selection

  gtk_box_append(GTK_BOX(target_source_selector), radio_equipment);
  gtk_box_append(GTK_BOX(target_source_selector), radio_manual_input);

  return target_source_selector;
}

static GtkWidget *create_equipment_search(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *container_search = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);

  GtkWidget *label_equipment = gtk_label_new("TARGET (NAME / ID / IP / MAC)");
  gtk_widget_set_halign(label_equipment, GTK_ALIGN_START);
  gtk_widget_add_css_class(label_equipment, "field-unit-label");

  GtkWidget *equipment_search = gtk_search_entry_new();
  gtk_widget_add_css_class(equipment_search, "search-target");
  g_signal_connect(equipment_search, "search-changed", G_CALLBACK(on_search_equipment_activated), ui_connectivity);

  GtkWidget *equipment_list = gtk_list_box_new();
  gtk_widget_set_visible(equipment_list, FALSE);
  g_object_set_data(G_OBJECT(equipment_search), "list-equipments", equipment_list);

  gtk_box_append(GTK_BOX(container_search), label_equipment);
  gtk_box_append(GTK_BOX(container_search), equipment_search);
  gtk_box_append(GTK_BOX(container_search), equipment_list);

  return container_search;
}

static void create_ping_parameters_section(GtkWidget *grid)
{
  GtkWidget *count_field = create_unit_field("COUNT", "5", "pkts");
  GtkWidget *timeout_field = create_unit_field("TIMEOUT", "2", "sec");
  GtkWidget *packet_size_field = create_unit_field("PACKET SIZE", "56 bytes (Standard)", NULL);

  gtk_grid_attach(GTK_GRID(grid), count_field, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), timeout_field, 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), packet_size_field, 0, 3, 2, 1);
}

static void create_ping_actions(GtkWidget *grid)
{
  GtkWidget *run_ping_button = create_secondary_button("Run Ping", "assets/icon-start.svg", "secondary-button");

  GtkWidget *clear_form_button = create_secondary_button("Clear", NULL, "clear-button");

  gtk_grid_attach(GTK_GRID(grid), run_ping_button, 0, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), clear_form_button, 1, 4, 1, 1);
}

static GtkWidget *create_connectivity_terminal(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  GtkWidget *terminal_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_widget_set_size_request(terminal_header, -1, 44);
  gtk_widget_add_css_class(terminal_header, "terminal-header");

  GtkWidget *image = gtk_image_new_from_file("assets/terminal-control-close.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(image), 12);
  gtk_box_append(GTK_BOX(terminal_header), image);
 
  image = gtk_image_new_from_file("assets/terminal-control-minimize.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(image), 12);
  gtk_box_append(GTK_BOX(terminal_header), image);
  
  image = gtk_image_new_from_file("assets/terminal-control-maximize.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(image), 12);
  gtk_box_append(GTK_BOX(terminal_header), image);

  GtkWidget *copy_button = create_secondary_button("Copy", "assets/icon-copy.svg", "terminal-header-button");
  gtk_widget_set_halign(copy_button, GTK_ALIGN_END);
  gtk_widget_set_hexpand(copy_button, TRUE);

  GtkWidget *save_button = create_secondary_button("Save", "assets/icon-save.svg", "terminal-header-button");
  gtk_widget_set_halign(save_button, GTK_ALIGN_END);

  GtkWidget *label = gtk_label_new("noc-technician@netpulse: ~/tools");
  gtk_widget_add_css_class(label, "terminal-header-title");

  gtk_box_append(GTK_BOX(terminal_header), label);
  gtk_box_append(GTK_BOX(terminal_header), copy_button);
  gtk_box_append(GTK_BOX(terminal_header), save_button);

  GtkWidget *terminal_panel = gtk_text_view_new();
  gtk_widget_set_size_request(terminal_panel, -1, 400);
  gtk_widget_set_sensitive(terminal_panel, FALSE);
  gtk_widget_add_css_class(terminal_panel, "terminal-panel");

  GtkTextBuffer *terminal_buffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER(terminal_buffer), "$ ping -c 5 -W 2 -s 56 192.168.1.1", -1);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(terminal_panel), terminal_buffer);

  gtk_box_append(GTK_BOX(box), terminal_header);
  gtk_box_append(GTK_BOX(box), terminal_panel);

  return box;
}

static void synchronize_navigation(ui_connectivity_t *ui_connectivity, GtkWidget *button)
{
  const char *label = g_object_get_data(G_OBJECT(button), "target-page");
  gtk_stack_set_visible_child_name(GTK_STACK(ui_connectivity->stack), label);

  const char *icons_default[] = {
    "assets/icon-ping-default.svg",
    "assets/icon-traceroute-default.svg",
    "assets/icon-dns-lookup-default.svg",
    "assets/icon-arp-table-default.svg"
  };

  const char *icons_active[] = {
    "assets/icon-ping-active.svg",
    "assets/icon-traceroute-active.svg",
    "assets/icon-dns-lookup-active.svg",
    "assets/icon-arp-table-active.svg"
  };

  for (int i = 0; i < 4; i++)
  {
    GtkWidget *box = gtk_widget_get_first_child(ui_connectivity->sidebar_buttons[i]);
    GtkWidget *image = gtk_widget_get_first_child(box);

    bool is_active = (ui_connectivity->buttons[i] == button || ui_connectivity->sidebar_buttons[i] == button);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui_connectivity->buttons[i]), is_active);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui_connectivity->sidebar_buttons[i]), is_active);
    gtk_image_set_from_file(GTK_IMAGE(image), is_active ? icons_active[i] : icons_default[i]);
  }
}

static GtkWidget *create_equipment_container(equipment_node_t *node) 
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_set_margin_top(box, 12);
  gtk_widget_set_margin_bottom(box, 12);
  gtk_widget_set_margin_start(box, 16);

  GtkWidget *image;

  switch (node->data.status) 
  {
    case STATUS_FAILED:
      image = gtk_image_new_from_file("assets/status-failed.svg");
      break;
    case STATUS_MAINTENANCE:
      image = gtk_image_new_from_file("assets/status-maintenance.svg");
      break;
    case STATUS_OPERATIONAL:
      image = gtk_image_new_from_file("assets/status-operational.svg");
      break;
    case STATUS_DISABLED:
      image = gtk_image_new_from_file("assets/status-disabled.svg");
      break;
  }

  gtk_image_set_pixel_size(GTK_IMAGE(image), 6);

  GtkWidget *text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);

  GtkWidget *name = gtk_label_new(node->data.name);
  gtk_widget_set_halign(name, GTK_ALIGN_START);
  gtk_widget_add_css_class(name, "search-item-name");

  GtkWidget *ip_address = gtk_label_new(node->data.ip_address);
  gtk_widget_set_halign(ip_address, GTK_ALIGN_START);
  gtk_widget_add_css_class(ip_address, "search-item-ip");

  gtk_box_append(GTK_BOX(text_box), name);
  gtk_box_append(GTK_BOX(text_box), ip_address);

  gtk_box_append(GTK_BOX(box), image);
  gtk_box_append(GTK_BOX(box), text_box);

  return box;
}

static void on_menu_button_clicked(GtkButton *button, gpointer data)
{
  ui_connectivity_t *ui_connectivity = (ui_connectivity_t *)data;
  synchronize_navigation(ui_connectivity, GTK_WIDGET(button));
}

static void on_sidebar_button_clicked(GtkButton *button, gpointer data)
{
  ui_connectivity_t *ui_connectivity = (ui_connectivity_t *)data;
  synchronize_navigation(ui_connectivity, GTK_WIDGET(button));
}

static void on_target_source_selection_clicked(GtkButton *button, gpointer data)
{
  GtkWidget *stack = (GtkWidget *)data;

  const char *label = g_object_get_data(G_OBJECT(button), "target-source");
  gtk_stack_set_visible_child_name(GTK_STACK(stack), label);
}

static void on_search_equipment_activated(GtkSearchEntry *search, gpointer data)
{
  ui_connectivity_t *ui_connectivity = (ui_connectivity_t *)data;

  GtkWidget *list = g_object_get_data(G_OBJECT(search), "list-equipments");

  GtkWidget *child = gtk_widget_get_first_child(list);
  while (child != NULL)
  {
    gtk_list_box_remove(GTK_LIST_BOX(list), child);
    child = gtk_widget_get_first_child(list);
  }

  gtk_widget_set_visible(list, TRUE);

  const char *text = gtk_editable_get_text(GTK_EDITABLE(search));

  equipment_node_t *node = NULL;

  switch (detect_search_type(text)) 
  {
    case SEARCH_ID:
      node = (equipment_node_t *) hashmap_get(&ui_connectivity->application->id_index, text);
      break;
    case SEARCH_IP:
      node = (equipment_node_t *) hashmap_get(&ui_connectivity->application->ip_index, text);
      break;
    case SEARCH_MAC:
      node = (equipment_node_t *) hashmap_get(&ui_connectivity->application->mac_index, text);
      break;
    case SEARCH_INVALID:
      break;
  }

  if (node == NULL) 
  {
    gtk_widget_set_visible(list, FALSE);
  }

  else 
  {
    equipment_list_t filtered;
    equipment_list_init(&filtered);

    equipment_node_t *new = equipment_list_insert(&filtered, node->data);
    new->data = node->data;

    GtkWidget *item = create_equipment_container(node);

    GtkWidget *row = gtk_list_box_row_new();
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), item);
    gtk_widget_add_css_class(row, "search-row-item");

    gtk_list_box_append(GTK_LIST_BOX(list), row);

    equipment_list_destroy(&filtered);
  }
}
