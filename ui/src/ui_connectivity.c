#include "ui_connectivity.h"

#include "utils.h"
#include "ui_widgets.h"
#include "macros.h"
#include "ui_macros.h"
#include "controller.h"

static const char* DATA_IP_ADDRESS = "equipment-ip";

static GtkWidget *create_connectivity_side_bar(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_connectivity_header(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_menu_bar(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_page_ping(ui_connectivity_t *ui_connectivity);

static GtkWidget *create_ping_header(void);
static GtkWidget *create_target_source_selector(GtkWidget *stack);
static GtkWidget *create_equipment_search(ui_ping_configuration_t *ui_ping);
static void create_ping_parameters_section(GtkWidget *grid, ui_ping_configuration_t *ui_ping);
static void create_ping_actions(GtkWidget *grid, ui_ping_configuration_t *ui_ping);
static GtkWidget *create_connectivity_terminal(ui_ping_configuration_t *ui_ping);
static GtkWidget *create_equipment_container(equipment_t equipment);
static void create_connectivity_list_row(GtkWidget *list, equipment_t equipment);

static void synchronize_navigation(ui_connectivity_t *ui_connectivity, GtkWidget *button);

// Callbacks
static void on_menu_button_clicked(GtkButton *button, gpointer data);
static void on_sidebar_button_clicked(GtkButton *button, gpointer data);
static void on_search_equipment_activated(GtkSearchEntry *search, gpointer data);
static void on_target_source_selection_clicked(GtkButton *button, gpointer data);
static void on_run_ping_clicked(GtkButton *button, gpointer data);
static void on_equipment_row_selected(GtkListBox *list, GtkListBoxRow *row, gpointer data);


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

void ui_ping_update_list(ui_ping_configuration_t *ui_ping, equipment_t *equipments, int count)
{
  GtkWidget *child = gtk_widget_get_first_child(ui_ping->list);
  while (child != NULL)
  {
    gtk_list_box_remove(GTK_LIST_BOX(ui_ping->list), child);
    child = gtk_widget_get_first_child(ui_ping->list);
  }

  gtk_widget_set_visible(ui_ping->list, TRUE);

  if (equipments == NULL || count == 0) return;

  for (int i = 0; i < count; i++)
  {
    create_connectivity_list_row(ui_ping->list, equipments[i]);
  }
}

void ui_ping_set_result(ui_ping_configuration_t *ui_ping, ping_result_t *result)
{
  GtkTextBuffer *terminal_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui_ping->terminal));
  
  if (result == NULL)
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(terminal_buffer), "$ ping -c 5 -W 2 -s 56 192.168.1.1", -1);
  else 
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(terminal_buffer), result->output, -1);
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
  ui_ping_configuration_t *ui_ping = malloc(sizeof(ui_ping_configuration_t));
  ui_ping->application = ui_connectivity->application;
  ui_ping->source = SOURCE_SELECTION_SEARCH;

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
  g_object_set_data(G_OBJECT(target_stack), "ui-ping-context", ui_ping);

  GtkWidget *target_source_selector = create_target_source_selector(target_stack);

  GtkWidget *form_fields_grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(form_fields_grid), 16);
  gtk_grid_set_column_spacing(GTK_GRID(form_fields_grid), 16);

  GtkWidget *container_search = create_equipment_search(ui_ping);
  gtk_stack_add_named(GTK_STACK(target_stack), container_search, "target-registered-equipment");
  gtk_stack_set_visible_child_name(GTK_STACK(target_stack), "target-registered-equipment");
 
  ui_ping->manual_ip = create_unit_field("TARGET (IP ADDRESS)", "192.168.1.1", NULL);
  gtk_stack_add_named(GTK_STACK(target_stack), ui_ping->manual_ip, "target-manual-ip");

  gtk_grid_attach(GTK_GRID(form_fields_grid), target_stack, 0, 1, 2, 1);

  create_ping_parameters_section(form_fields_grid, ui_ping);
  create_ping_actions(form_fields_grid, ui_ping);

  gtk_box_append(GTK_BOX(config_panel), panel_header);
  gtk_box_append(GTK_BOX(config_panel), label_target);
  gtk_box_append(GTK_BOX(config_panel), target_source_selector);
  gtk_box_append(GTK_BOX(config_panel), form_fields_grid);

  GtkWidget *terminal_panel = create_connectivity_terminal(ui_ping);
  gtk_box_append(GTK_BOX(container), config_panel);
  gtk_box_append(GTK_BOX(container), terminal_panel);

  g_object_set_data_full(G_OBJECT(container), "ui-ping-context", ui_ping, free);  // ownership + free

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

  GtkWidget *radio_equipment = gtk_check_button_new_with_label("Registered Equipment");
  gtk_widget_add_css_class(radio_equipment, "config-panel-form-checkbutton");
  g_object_set_data(G_OBJECT(radio_equipment), "target-source", "target-registered-equipment");
  gtk_check_button_set_active(GTK_CHECK_BUTTON(radio_equipment), TRUE);
  g_signal_connect(radio_equipment, "toggled", G_CALLBACK(on_target_source_selection_clicked), stack);

  GtkWidget *radio_manual_input = gtk_check_button_new_with_label("Manual IP Address");
  gtk_widget_add_css_class(radio_manual_input, "config-panel-form-checkbutton");
  g_object_set_data(G_OBJECT(radio_manual_input), "target-source", "target-manual-ip");
  g_signal_connect(radio_manual_input, "toggled", G_CALLBACK(on_target_source_selection_clicked), stack);

  gtk_check_button_set_group(GTK_CHECK_BUTTON(radio_equipment), GTK_CHECK_BUTTON(radio_manual_input)); // mutually exclusive selection

  gtk_box_append(GTK_BOX(target_source_selector), radio_equipment);
  gtk_box_append(GTK_BOX(target_source_selector), radio_manual_input);

  return target_source_selector;
}

static GtkWidget *create_equipment_search(ui_ping_configuration_t *ui_ping)
{
  GtkWidget *container_search = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);

  GtkWidget *label_equipment = gtk_label_new("TARGET (ID / IP / MAC)");
  gtk_widget_set_halign(label_equipment, GTK_ALIGN_START);
  gtk_widget_add_css_class(label_equipment, "field-unit-label");

  GtkWidget *equipment_search = gtk_search_entry_new();
  gtk_widget_add_css_class(equipment_search, "search-target");
  g_signal_connect(equipment_search, "search-changed", G_CALLBACK(on_search_equipment_activated), ui_ping);

  ui_ping->list = gtk_list_box_new();
  gtk_widget_set_visible(ui_ping->list, FALSE);
  g_signal_connect(ui_ping->list, "row-selected", G_CALLBACK(on_equipment_row_selected), ui_ping);

  gtk_box_append(GTK_BOX(container_search), label_equipment);
  gtk_box_append(GTK_BOX(container_search), equipment_search);
  gtk_box_append(GTK_BOX(container_search), ui_ping->list);

  return container_search;
}

static void create_ping_parameters_section(GtkWidget *grid, ui_ping_configuration_t *ui_ping)
{
  ui_ping->count = create_unit_field("COUNT", "5", "pkts");
  ui_ping->timeout = create_unit_field("TIMEOUT", "2", "sec");
  ui_ping->packet_size = create_unit_field("PACKET SIZE", "56 bytes (Standard)", NULL);

  gtk_grid_attach(GTK_GRID(grid), ui_ping->count, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), ui_ping->timeout, 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), ui_ping->packet_size, 0, 3, 2, 1);
}

static void create_ping_actions(GtkWidget *grid, ui_ping_configuration_t *ui_ping)
{
  GtkWidget *run_ping_button = create_secondary_button("Run Ping", "assets/icon-start.svg", "secondary-button");
  g_signal_connect(run_ping_button, "clicked", G_CALLBACK(on_run_ping_clicked), ui_ping);

  GtkWidget *clear_form_button = create_secondary_button("Clear", NULL, "clear-button");

  gtk_grid_attach(GTK_GRID(grid), run_ping_button, 0, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), clear_form_button, 1, 4, 1, 1);
}

static GtkWidget *create_connectivity_terminal(ui_ping_configuration_t *ui_ping)
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

  ui_ping->terminal = gtk_text_view_new();
  gtk_widget_set_size_request(ui_ping->terminal, -1, 400);
  gtk_widget_set_sensitive(ui_ping->terminal, FALSE);
  gtk_widget_add_css_class(ui_ping->terminal, "terminal-panel");

  GtkTextBuffer *terminal_buffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER(terminal_buffer), "$ ping -c 5 -W 2 -s 56 192.168.1.1", -1);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(ui_ping->terminal), terminal_buffer);

  gtk_box_append(GTK_BOX(box), terminal_header);
  gtk_box_append(GTK_BOX(box), ui_ping->terminal);

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

static GtkWidget *create_equipment_container(equipment_t equipment) 
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_set_margin_top(box, 12);
  gtk_widget_set_margin_bottom(box, 12);
  gtk_widget_set_margin_start(box, 16);

  GtkWidget *image;

  switch (equipment.status) 
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

  GtkWidget *name = gtk_label_new(equipment.name);
  gtk_widget_set_halign(name, GTK_ALIGN_START);
  gtk_widget_add_css_class(name, "search-item-name");

  GtkWidget *ip_address = gtk_label_new(equipment.ip_address);
  gtk_widget_set_halign(ip_address, GTK_ALIGN_START);
  gtk_widget_add_css_class(ip_address, "search-item-ip");

  gtk_box_append(GTK_BOX(text_box), name);
  gtk_box_append(GTK_BOX(text_box), ip_address);

  gtk_box_append(GTK_BOX(box), image);
  gtk_box_append(GTK_BOX(box), text_box);

  return box;
}



static void create_connectivity_list_row(GtkWidget *list, equipment_t equipment)
{
  GtkWidget *item = create_equipment_container(equipment);

  GtkWidget *row = gtk_list_box_row_new();

  char *ip_address = strdup(equipment.ip_address);
  g_object_set_data_full(G_OBJECT(row), DATA_IP_ADDRESS, ip_address, free); // ownership + free automatic

  gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), item);
  gtk_widget_add_css_class(row, "search-row-item");

  gtk_list_box_append(GTK_LIST_BOX(list), row);
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
  ui_ping_configuration_t *ui_ping = g_object_get_data(G_OBJECT(stack), "ui-ping-context");

  const char *label = g_object_get_data(G_OBJECT(button), "target-source");
  gtk_stack_set_visible_child_name(GTK_STACK(stack), label);

  ui_ping->source = (strcmp(label, "target-registered-equipment") == 0) ? SOURCE_SELECTION_SEARCH : SOURCE_SELECTION_MANUAL;
}

static void on_search_equipment_activated(GtkSearchEntry *search, gpointer data)
{
  ui_ping_configuration_t *ui_ping = (ui_ping_configuration_t *)data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(search));

  connectivity_controller_search_equipment(ui_ping, text);
}

static void on_equipment_row_selected(GtkListBox *list, GtkListBoxRow *row, gpointer data)
{
  (void)list; // unused variable
  
  ui_ping_configuration_t *ui_ping = (ui_ping_configuration_t *)data;

  if (row == NULL) return;

  const char *ip_address = g_object_get_data(G_OBJECT(row), DATA_IP_ADDRESS);
  snprintf(ui_ping->target_ip, IP_MAX, "%s", ip_address);
}

static void on_run_ping_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused variable

  ui_ping_configuration_t *ui_ping = (ui_ping_configuration_t *)data; 

  GtkWidget *ip_entry = g_object_get_data(G_OBJECT(ui_ping->manual_ip), "entry");
  GtkWidget *count_entry = g_object_get_data(G_OBJECT(ui_ping->count), "entry");
  GtkWidget *timeout_entry = g_object_get_data(G_OBJECT(ui_ping->timeout), "entry");
  GtkWidget *packet_size_entry = g_object_get_data(G_OBJECT(ui_ping->packet_size), "entry");

  if (ui_ping->source == SOURCE_SELECTION_MANUAL)
  {
    const char *ip_text = gtk_editable_get_text(GTK_EDITABLE(ip_entry));
    snprintf(ui_ping->target_ip, IP_MAX, "%s", ip_text);
  }

  const char *count = gtk_editable_get_text(GTK_EDITABLE(count_entry));
  const char *timeout = gtk_editable_get_text(GTK_EDITABLE(timeout_entry));
  const char *packet_size = gtk_editable_get_text(GTK_EDITABLE(packet_size_entry));
 
  ping_validation_t error = connectivity_controller_validate_ping(ui_ping->target_ip, count, timeout, packet_size);

  switch (error) 
  {
    case PING_INVALID_IP_ADDRESS:
      gtk_widget_add_css_class(ui_ping->manual_ip, "field-error");
      break;
    case PING_INVALID_COUNT:
      gtk_widget_add_css_class(ui_ping->count, "field-error");
      break;
    case PING_INVALID_TIMEOUT:
      gtk_widget_add_css_class(ui_ping->timeout, "field-error");
      break;
    case PING_INVALID_PACKET_SIZE:
      gtk_widget_add_css_class(ui_ping->packet_size, "field-error");
      break;
    case PING_OK:
      connectivity_controller_ping(ui_ping, ui_ping->target_ip, count, timeout, packet_size);
      break;
  }
}
