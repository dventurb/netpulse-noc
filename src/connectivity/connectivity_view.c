#include "connectivity_view.h"

#include "utils.h"
#include "ui_widgets.h"
#include "macros.h"

static const char* DATA_IP_ADDRESS = "equipment-ip";

static GtkWidget *build_sidebar(connectivity_view_t *view);
static GtkWidget *build_header(connectivity_view_t *view);
static GtkWidget *build_menu_bar(connectivity_view_t *view);

static GtkBox *ping_view_create(ping_view_t *view, connectivity_controller_t *controller);

static GtkWidget *build_ping_header(void);
static GtkWidget *build_source_selection(ping_view_t *view);
static GtkWidget *build_search(ping_view_t *view);
static void build_parameters_section(GtkWidget *grid, ping_view_t *view);
static void build_actions(GtkWidget *grid, ping_view_t *view);
static GtkWidget *build_terminal(ping_view_t *view);
static GtkWidget *build_equipment_cell(equipment_t equipment);
static void build_list_row(GtkWidget *list, equipment_t equipment);

static void synchronize_navigation(connectivity_view_t *view, GtkWidget *button);

// Callbacks
static void on_menu_button_clicked(GtkButton *button, gpointer data);
static void on_sidebar_button_clicked(GtkButton *button, gpointer data);

static void on_search_equipment_activated(GtkSearchEntry *search, gpointer data);
static void on_target_source_selection_clicked(GtkButton *button, gpointer data);
static void on_equipment_row_selected(GtkListBox *list, GtkListBoxRow *row, gpointer data);
static void on_run_ping_clicked(GtkButton *button, gpointer data);
static void on_ping_all_clicked(GtkButton *button, gpointer data);

GtkBox *connectivity_view_create(connectivity_view_t *view, connectivity_controller_t *controller)
{
  view->controller = controller;

  view->container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
  view->stack = GTK_STACK(gtk_stack_new());

  GtkWidget *side_bar = build_sidebar(view);

  GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  GtkWidget *header = build_header(view);

  GtkWidget *ping_container = GTK_WIDGET(ping_view_create(&view->ping_view, view->controller));
  gtk_stack_add_named(view->stack, ping_container, "PING");
 
  gtk_stack_set_visible_child_name(view->stack, "PING");
  
  gtk_box_append(GTK_BOX(right_box), header);
  gtk_box_append(GTK_BOX(right_box), GTK_WIDGET(view->stack));

  gtk_box_append(view->container, side_bar);
  gtk_box_append(view->container, right_box);

  return view->container;
}

void ping_view_update_list(ping_view_t *view, equipment_t *equipments, int count)
{
  remove_list_rows(GTK_WIDGET(view->list));

  gtk_widget_set_visible(GTK_WIDGET(view->list), TRUE);

  if (equipments == NULL || count == 0) return;

  for (int i = 0; i < count; i++)
  {
    build_list_row(GTK_WIDGET(view->list), equipments[i]);
  }
}

void ping_view_set_result(ping_view_t *view, const char *output)
{
  ping_view_set_actions_enabled(view, true);

  if (output == NULL) return;

  char *utf8_char = g_utf8_make_valid(output, -1);

  GtkTextBuffer *terminal_buffer = gtk_text_view_get_buffer(view->terminal);

  GtkTextIter end;
  gtk_text_buffer_get_end_iter(terminal_buffer, &end);

  gtk_text_buffer_insert(terminal_buffer, &end, utf8_char, -1);

  free(utf8_char);
}

void ping_view_clear_result(ping_view_t *view)
{
  GtkTextBuffer *terminal_buffer = gtk_text_view_get_buffer(view->terminal);
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER(terminal_buffer), "$ ", -1);
}

void ping_view_set_actions_enabled(ping_view_t *view, bool is_active)
{
  gtk_widget_set_sensitive(GTK_WIDGET(view->run_button), is_active);
  gtk_widget_set_sensitive(GTK_WIDGET(view->ping_all_button), is_active);
}

static GtkWidget *build_sidebar(connectivity_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 256, -1);
  gtk_widget_set_vexpand(box, TRUE);
  gtk_widget_add_css_class(box, "connectivity-sidebar");

  GtkWidget *label = gtk_label_new("NETWORK TOOLS");
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_widget_add_css_class(label, "connectivity-sidebar-label");
  gtk_box_append(GTK_BOX(box), label);

  const char *tools[CONNECTIVITY_PAGE_COUNT] = {
    "Ping",
    "TraceRoute",
    "DNS Lookup",
    "ARP"
  };

  const char *icons[CONNECTIVITY_PAGE_COUNT] = {
    "assets/icon-ping-active.svg",
    "assets/icon-traceroute-default.svg",
    "assets/icon-dns-lookup-default.svg",
    "assets/icon-arp-table-default.svg"
  };

  for (int i = 0; i < CONNECTIVITY_PAGE_COUNT; i++)
  {
    view->sidebar_buttons[i] = GTK_TOGGLE_BUTTON(create_primary_button(tools[i], icons[i],"connectivity-sidebar-button"));

    g_object_set_data(G_OBJECT(view->sidebar_buttons[i]), "target-page", (void *)tools[i]);

    g_signal_connect(GTK_WIDGET(view->sidebar_buttons[i]), "clicked", G_CALLBACK(on_sidebar_button_clicked), view);

    gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->sidebar_buttons[i]));
  }

  gtk_toggle_button_set_active(view->sidebar_buttons[0], TRUE);

  return box;
}

static GtkWidget *build_header(connectivity_view_t *view)
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

  GtkWidget *menu_bar = build_menu_bar(view);
  gtk_widget_set_margin_start(menu_bar, 32);

  gtk_box_append(GTK_BOX(box), title_box);
  gtk_box_append(GTK_BOX(box), menu_bar);

  return box;
}

static GtkWidget *build_menu_bar(connectivity_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 24);

  const char *tools[CONNECTIVITY_PAGE_COUNT] = {
    "Ping",
    "TraceRoute",
    "DNS Lookup",
    "ARP"
  };

  for (int i = 0; i < CONNECTIVITY_PAGE_COUNT; i++)
  {
    view->buttons[i] = GTK_TOGGLE_BUTTON(create_primary_button(tools[i], NULL, "connectivity-tools-button"));

    g_object_set_data(G_OBJECT(view->buttons[i]), "target-page", (void *)tools[i]);

    g_signal_connect(GTK_WIDGET(view->buttons[i]), "clicked", G_CALLBACK(on_menu_button_clicked), view);

    gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->buttons[i]));
  }

  gtk_toggle_button_set_active(view->buttons[0], TRUE);

  return box;
}

static GtkBox *ping_view_create(ping_view_t *view, connectivity_controller_t *controller)
{
  view->controller = controller;

  view->container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 24));
  gtk_widget_set_margin_top(GTK_WIDGET(view->container), 32);
  gtk_widget_set_margin_start(GTK_WIDGET(view->container), 32);
  gtk_widget_set_margin_end(GTK_WIDGET(view->container), 32);

  GtkWidget *config_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_widget_set_size_request(config_panel, -1, 400);
  gtk_widget_add_css_class(config_panel, "connectivity-config-panel");

  GtkWidget *panel_header = build_ping_header();

  GtkWidget *label_target = gtk_label_new("TARGET SOURCE SELECTION");
  gtk_widget_set_halign(label_target, GTK_ALIGN_START);
  gtk_widget_add_css_class(label_target, "config-panel-form-label");

  view->stack = GTK_STACK(gtk_stack_new());

  GtkWidget *target_source_selector = build_source_selection(view);

  GtkWidget *form_fields_grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(form_fields_grid), 16);
  gtk_grid_set_column_spacing(GTK_GRID(form_fields_grid), 16);

  GtkWidget *container_search = build_search(view);
  gtk_stack_add_named(view->stack, container_search, "REGISTERED");

  gtk_stack_set_visible_child_name(view->stack, "REGISTERED");
 
  view->manual_ip = create_unit_field("TARGET (IP ADDRESS)", "192.168.1.1", NULL);
  gtk_stack_add_named(view->stack, view->manual_ip, "MANUAL");

  gtk_grid_attach(GTK_GRID(form_fields_grid), GTK_WIDGET(view->stack), 0, 1, 2, 1);

  build_parameters_section(form_fields_grid, view);
  build_actions(form_fields_grid, view);

  gtk_box_append(GTK_BOX(config_panel), panel_header);
  gtk_box_append(GTK_BOX(config_panel), label_target);
  gtk_box_append(GTK_BOX(config_panel), target_source_selector);
  gtk_box_append(GTK_BOX(config_panel), form_fields_grid);

  GtkWidget *terminal_panel = build_terminal(view);

  gtk_box_append(view->container, config_panel);
  gtk_box_append(view->container, terminal_panel);

  return view->container;
}

static GtkWidget *build_ping_header(void)
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

static GtkWidget *build_source_selection(ping_view_t *view)
{
  GtkWidget *target_source_selector = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);

  view->registered_button = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Registered Equipment"));
  gtk_widget_add_css_class(GTK_WIDGET(view->registered_button), "config-panel-form-checkbutton");
  gtk_check_button_set_active(view->registered_button, TRUE);
  g_signal_connect(GTK_WIDGET(view->registered_button), "toggled", G_CALLBACK(on_target_source_selection_clicked), view);

  view->manual_button = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Manual IP Address"));
  gtk_widget_add_css_class(GTK_WIDGET(view->manual_button), "config-panel-form-checkbutton");
  g_signal_connect(GTK_WIDGET(view->manual_button), "toggled", G_CALLBACK(on_target_source_selection_clicked), view);

  gtk_check_button_set_group(view->registered_button, view->manual_button); // mutually exclusive selection

  gtk_box_append(GTK_BOX(target_source_selector), GTK_WIDGET(view->registered_button));
  gtk_box_append(GTK_BOX(target_source_selector), GTK_WIDGET(view->manual_button));

  return target_source_selector;
}

static GtkWidget *build_search(ping_view_t *view)
{
  GtkWidget *container_search = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);

  GtkWidget *label_equipment = gtk_label_new("TARGET (ID / IP / MAC)");
  gtk_widget_set_halign(label_equipment, GTK_ALIGN_START);
  gtk_widget_add_css_class(label_equipment, "field-unit-label");

  GtkWidget *equipment_search = gtk_search_entry_new();
  gtk_widget_add_css_class(equipment_search, "search-target");
  g_signal_connect(equipment_search, "search-changed", G_CALLBACK(on_search_equipment_activated), view);

  view->list = GTK_LIST_BOX(gtk_list_box_new());
  gtk_widget_set_visible(GTK_WIDGET(view->list), FALSE);
  g_signal_connect(GTK_WIDGET(view->list), "row-selected", G_CALLBACK(on_equipment_row_selected), view);

  gtk_box_append(GTK_BOX(container_search), label_equipment);
  gtk_box_append(GTK_BOX(container_search), equipment_search);
  gtk_box_append(GTK_BOX(container_search), GTK_WIDGET(view->list));

  return container_search;
}

static void build_parameters_section(GtkWidget *grid, ping_view_t *view)
{
  view->count = create_unit_field("COUNT", "5", "pkts");
  view->timeout = create_unit_field("TIMEOUT", "2", "sec");
  view->packet_size = create_unit_field("PACKET SIZE", "56 bytes (Standard)", NULL);

  gtk_grid_attach(GTK_GRID(grid), view->count, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), view->timeout, 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), view->packet_size, 0, 3, 2, 1);
}

static void build_actions(GtkWidget *grid, ping_view_t *view)
{
  view->run_button = GTK_BUTTON(create_secondary_button("Run Ping", "assets/icon-start.svg", "secondary-button"));
  g_signal_connect(GTK_WIDGET(view->run_button), "clicked", G_CALLBACK(on_run_ping_clicked), view);

  view->ping_all_button = GTK_BUTTON(create_secondary_button("Ping All Equipments", NULL, "ping-all-button"));
  g_signal_connect(GTK_WIDGET(view->ping_all_button), "clicked", G_CALLBACK(on_ping_all_clicked), view);

  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->run_button), 0, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->ping_all_button), 1, 4, 1, 1);
}

static GtkWidget *build_terminal(ping_view_t *view)
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

  GtkWidget *scrolled = gtk_scrolled_window_new();
  gtk_widget_set_size_request(scrolled, -1, 400);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  view->terminal = GTK_TEXT_VIEW(gtk_text_view_new());
  gtk_widget_set_size_request(GTK_WIDGET(view->terminal), -1, 400);
  gtk_widget_set_sensitive(GTK_WIDGET(view->terminal), FALSE);
  gtk_widget_add_css_class(GTK_WIDGET(view->terminal), "terminal-panel");

  GtkTextBuffer *terminal_buffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER(terminal_buffer), "$ ping -c 5 -W 2 -s 56 192.168.1.1", -1);

  gtk_text_view_set_buffer(view->terminal, terminal_buffer);

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), GTK_WIDGET(view->terminal));

  gtk_box_append(GTK_BOX(box), terminal_header);
  gtk_box_append(GTK_BOX(box), scrolled);

  return box;
}

static void synchronize_navigation(connectivity_view_t *view, GtkWidget *button)
{
  const char *label = g_object_get_data(G_OBJECT(button), "target-page");
  gtk_stack_set_visible_child_name(view->stack, label);

  const char *icons_default[CONNECTIVITY_PAGE_COUNT] = {
    "assets/icon-ping-default.svg",
    "assets/icon-traceroute-default.svg",
    "assets/icon-dns-lookup-default.svg",
    "assets/icon-arp-table-default.svg"
  };

  const char *icons_active[CONNECTIVITY_PAGE_COUNT] = {
    "assets/icon-ping-active.svg",
    "assets/icon-traceroute-active.svg",
    "assets/icon-dns-lookup-active.svg",
    "assets/icon-arp-table-active.svg"
  };

  for (int i = 0; i < CONNECTIVITY_PAGE_COUNT; i++)
  {
    GtkWidget *box = gtk_widget_get_first_child(GTK_WIDGET(view->sidebar_buttons[i]));
    GtkWidget *image = gtk_widget_get_first_child(box);

    bool is_active = (GTK_WIDGET(view->buttons[i]) == button || GTK_WIDGET(view->sidebar_buttons[i]) == button);

    gtk_toggle_button_set_active(view->buttons[i], is_active);
    gtk_toggle_button_set_active(view->sidebar_buttons[i], is_active);

    gtk_image_set_from_file(GTK_IMAGE(image), is_active ? icons_active[i] : icons_default[i]);
  }
}

static GtkWidget *build_equipment_cell(equipment_t equipment) 
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

static void build_list_row(GtkWidget *list, equipment_t equipment)
{
  GtkWidget *item = build_equipment_cell(equipment);

  GtkWidget *row = gtk_list_box_row_new();

  char *ip_address = strdup(equipment.ip_address);
  g_object_set_data_full(G_OBJECT(row), DATA_IP_ADDRESS, ip_address, free); // ownership + free automatic

  gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), item);
  gtk_widget_add_css_class(row, "search-row-item");

  gtk_list_box_append(GTK_LIST_BOX(list), row);
}

static void on_menu_button_clicked(GtkButton *button, gpointer data)
{
  connectivity_view_t *view = (connectivity_view_t *)data;
  synchronize_navigation(view, GTK_WIDGET(button));
}

static void on_sidebar_button_clicked(GtkButton *button, gpointer data)
{
  connectivity_view_t *view = (connectivity_view_t *)data;
  synchronize_navigation(view, GTK_WIDGET(button));
}

static void on_target_source_selection_clicked(GtkButton *button, gpointer data)
{
  ping_view_t *view = (ping_view_t *)data;

  target_source_t source;

  if (GTK_WIDGET(view->registered_button) == GTK_WIDGET(button))
  {
    gtk_stack_set_visible_child_name(view->stack, "REGISTERED");
    source = SOURCE_SEARCH;
  }

  else 
  {
    gtk_stack_set_visible_child_name(view->stack, "MANUAL");
    source = SOURCE_MANUAL;
  }

  connectivity_controller_set_source_selection(view->controller, source);
}

static void on_search_equipment_activated(GtkSearchEntry *search, gpointer data)
{
  ping_view_t *view = (ping_view_t *)data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(search));

  connectivity_controller_search_equipment(view->controller, text);
}

static void on_equipment_row_selected(GtkListBox *list, GtkListBoxRow *row, gpointer data)
{
  (void)list; // unused variable
  
  ping_view_t *view = (ping_view_t *)data;
  
  if (row == NULL) return;

  const char *ip_address = g_object_get_data(G_OBJECT(row), DATA_IP_ADDRESS);

  connectivity_controller_set_ip_address(view->controller, ip_address);
}

static void on_run_ping_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused

  ping_view_t *view = (ping_view_t *)data; 

  ping_view_clear_result(view);
  
  ping_view_set_actions_enabled(view, false);

  GtkWidget *ip_entry = g_object_get_data(G_OBJECT(view->manual_ip), "entry");
  GtkWidget *count_entry = g_object_get_data(G_OBJECT(view->count), "entry");
  GtkWidget *timeout_entry = g_object_get_data(G_OBJECT(view->timeout), "entry");
  GtkWidget *packet_size_entry = g_object_get_data(G_OBJECT(view->packet_size), "entry");

  const char *ip_text = gtk_editable_get_text(GTK_EDITABLE(ip_entry));
  const char *count = gtk_editable_get_text(GTK_EDITABLE(count_entry));
  const char *timeout = gtk_editable_get_text(GTK_EDITABLE(timeout_entry));
  const char *packet_size = gtk_editable_get_text(GTK_EDITABLE(packet_size_entry));

  connectivity_controller_set_ip_from_source(view->controller, ip_text);
 
  ping_validation_t error = connectivity_controller_validate_ping(view->controller->ip, count, timeout, packet_size);

  switch (error) 
  {
    case PING_INVALID_IP_ADDRESS:
      gtk_widget_add_css_class(view->manual_ip, "field-error");
      break;
    case PING_INVALID_COUNT:
      gtk_widget_add_css_class(view->count, "field-error");
      break;
    case PING_INVALID_TIMEOUT:
      gtk_widget_add_css_class(view->timeout, "field-error");
      break;
    case PING_INVALID_PACKET_SIZE:
      gtk_widget_add_css_class(view->packet_size, "field-error");
      break;
    case PING_OK:
      connectivity_controller_ping(view->controller, count, timeout, packet_size);
      break;
  }

  if (error != PING_OK) 
    ping_view_set_actions_enabled(view, true);
}

static void on_ping_all_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused 

  ping_view_t *view = (ping_view_t *)data; 

  ping_view_set_actions_enabled(view, false);
  
  ping_view_clear_result(view);
  
  connectivity_controller_ping_all(view->controller);
}
