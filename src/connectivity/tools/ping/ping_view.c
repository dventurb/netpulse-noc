#include "ping_view.h"

#include "widget_utils.h"
#include "list_item.h"

static const char* DATA_IP_ADDRESS = "EQUIPMENT-IP";

static void build_layout(ping_view_t *view);

static GtkWidget *build_top_panel(ping_view_t *view);
static GtkWidget *build_terminal_panel(ping_view_t *view);

static GtkWidget *build_configuration_panel(ping_view_t *view);
static GtkWidget *build_cards_panel(ping_view_t *view);

static GtkWidget *build_configuration_header(void);
static GtkWidget *build_source_selector(ping_view_t *view);
static GtkWidget *build_ping_parameters(ping_view_t *view);
static GtkWidget *build_search_target(ping_view_t *view);

static void build_list_row(GtkWidget *list, equipment_t equipment);
static GtkWidget *build_equipment_cell(equipment_t equipment);

// Callbacks 
static void on_source_selector_clicked(GtkButton *button, gpointer data);
static void on_search_equipment_activated(GtkSearchEntry *search, gpointer data);
static void on_equipment_row_selected(GtkListBox *list, GtkListBoxRow *row, gpointer data);

static void on_run_ping_clicked(GtkButton *button, gpointer data);
static void on_run_ping_all_equipments_clicked(GtkButton *button, gpointer data);

static void on_save_output_clicked(GtkButton *button, gpointer data);
static void on_copy_output_clicked(GtkButton *button, gpointer data);
static void on_save_output_finish(GObject *self, GAsyncResult *res, gpointer data);


GtkBox *ping_view_create(ping_view_t *view, ping_controller_t *controller)
{
  view->controller = controller;

  view->container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

  build_layout(view);

  return view->container;
}

static void build_layout(ping_view_t *view)
{
  GtkWidget *scrolled = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_widget_add_css_class(scrolled, "table-scroll");

  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_margin_top(container, 32);
  gtk_widget_set_margin_start(container, 32);
  gtk_widget_set_margin_end(container, 32);

  GtkWidget *top_panel = build_top_panel(view);
  GtkWidget *terminal_panel = build_terminal_panel(view);

  gtk_box_append(GTK_BOX(container), top_panel);
  gtk_box_append(GTK_BOX(container), terminal_panel);

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), container);

  gtk_box_append(view->container, scrolled);
}

static GtkWidget *build_top_panel(ping_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);

  GtkWidget *config_panel = build_configuration_panel(view);
  GtkWidget *cards_panel = build_cards_panel(view);

  gtk_box_append(GTK_BOX(container), config_panel);
  gtk_box_append(GTK_BOX(container), cards_panel);

  return container;
}

static GtkWidget *build_terminal_panel(ping_view_t *view)
{
  view->terminal = terminal_panel_new("noc-technician@netpulse: ~/tools/ping");

  g_signal_connect(view->terminal.copy_button, "clicked", G_CALLBACK(on_copy_output_clicked), view);
  g_signal_connect(view->terminal.save_button, "clicked", G_CALLBACK(on_save_output_clicked), view);  

  return GTK_WIDGET(view->terminal.container);
}

static GtkWidget *build_configuration_panel(ping_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_widget_set_size_request(container, -1, 400);
  gtk_widget_add_css_class(container, "connectivity-config-panel");

  GtkWidget *header = build_configuration_header();

  GtkWidget *label = gtk_label_new("TARGET SOURCE SELECTION");
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_add_css_class(label, "config-panel-form-label");

  view->stack = GTK_STACK(gtk_stack_new());
  gtk_stack_set_vhomogeneous(view->stack, FALSE);
 
  GtkWidget *source_selector = build_source_selector(view);

  GtkWidget *parameters = build_ping_parameters(view);

  GtkWidget *search_target = build_search_target(view);
  gtk_stack_add_named(view->stack, search_target, "REGISTERED");

  view->manual_ip_field = input_field_new("TARGET (IP ADDRESS)", "192.168.1.1", NULL);
  gtk_widget_add_css_class(GTK_WIDGET(view->manual_ip_field.container), "target-field-label");
  gtk_stack_add_named(view->stack, GTK_WIDGET(view->manual_ip_field.container), "MANUAL");

  gtk_stack_set_visible_child_name(view->stack, "REGISTERED");

  gtk_box_append(GTK_BOX(container), header);
  gtk_box_append(GTK_BOX(container), label);
  gtk_box_append(GTK_BOX(container), source_selector);
  gtk_box_append(GTK_BOX(container), parameters);

  return container;
}

static GtkWidget *build_cards_panel(ping_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
  gtk_box_set_homogeneous(GTK_BOX(container), FALSE);
  gtk_widget_set_hexpand(container, TRUE);

  GtkWidget *left_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 40);
  gtk_box_set_homogeneous(GTK_BOX(left_container), FALSE);
  gtk_widget_set_hexpand(left_container, TRUE);

  view->status_card = info_card_new("TARGET STATUS", "assets/icon-responded.svg", "Online", "192.168.1.1");
  view->packets_card = info_card_new("PACKET LOSS", "assets/icon-packet.svg", "0%", "0 transmitted, 0 received");

  gtk_box_append(GTK_BOX(left_container), GTK_WIDGET(view->status_card.container));
  gtk_box_append(GTK_BOX(left_container), GTK_WIDGET(view->packets_card.container));

  GtkWidget *right_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 40);
  gtk_box_set_homogeneous(GTK_BOX(right_container), FALSE);
  gtk_widget_set_hexpand(right_container, TRUE);
  
  view->average_card = info_card_new("AVG LATENCY", "assets/icon-timer.svg", "0.0", "0ms from baseline");
  view->execution_card = info_card_new("LAST EXECUTION", "assets/icon-clock.svg", "01-01-01 00:00", "Duration: 0.00s");

  gtk_box_append(GTK_BOX(right_container), GTK_WIDGET(view->average_card.container));
  gtk_box_append(GTK_BOX(right_container), GTK_WIDGET(view->execution_card.container));

  gtk_box_append(GTK_BOX(container), left_container);
  gtk_box_append(GTK_BOX(container), right_container);

  return container;
}

static GtkWidget *build_configuration_header(void)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_add_css_class(container, "config_panel-header");
  gtk_widget_set_valign(container, GTK_ALIGN_START);
  
  GtkWidget *header_icon = gtk_image_new_from_file("assets/icon-ping-configuration.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(header_icon), 14);

  GtkWidget *header_title = gtk_label_new("Ping Configuration");
  gtk_widget_add_css_class(header_title, "config-panel-title");

  gtk_box_append(GTK_BOX(container), header_icon);
  gtk_box_append(GTK_BOX(container), header_title);

  return container;
}

static GtkWidget *build_source_selector(ping_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);

  view->registered_button = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Registered Equipment"));
  gtk_widget_add_css_class(GTK_WIDGET(view->registered_button), "config-panel-form-checkbutton");
  gtk_check_button_set_active(view->registered_button, TRUE);
  g_signal_connect(GTK_WIDGET(view->registered_button), "toggled", G_CALLBACK(on_source_selector_clicked), view);

  view->manual_button = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Manual IP Address"));
  gtk_widget_add_css_class(GTK_WIDGET(view->manual_button), "config-panel-form-checkbutton");
  g_signal_connect(GTK_WIDGET(view->manual_button), "toggled", G_CALLBACK(on_source_selector_clicked), view);

  /* mutually exclusive selection */
  gtk_check_button_set_group(view->registered_button, view->manual_button);   

  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->registered_button));
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->manual_button));

  return container;
}

static GtkWidget *build_ping_parameters(ping_view_t *view)
{
  GtkWidget *container = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(container), 16);
  gtk_grid_set_column_spacing(GTK_GRID(container), 16);

  view->count_field = unit_field_new("COUNT", "5", "pkts");
  gtk_entry_set_max_length(view->count_field.entry, PING_COUNT_MAX - 1);

  view->timeout_field = unit_field_new("TIMEOUT", "2", "sec");
  gtk_entry_set_max_length(view->timeout_field.entry, PING_TIMEOUT_MAX - 1);

  view->packet_size_field = unit_field_new("PACKET SIZE", "56 bytes (Standard)", NULL);
  gtk_entry_set_max_length(view->packet_size_field.entry, PING_PACKET_SIZE_MAX - 1);

  view->run_button = GTK_BUTTON(action_button_new("Run Ping", "assets/icon-start.svg", "secondary-button"));
  view->ping_all_button = GTK_BUTTON(action_button_new("Ping All Equipments", NULL, "ping-all-button"));

  g_signal_connect(GTK_WIDGET(view->run_button), "clicked", G_CALLBACK(on_run_ping_clicked), view);
  g_signal_connect(GTK_WIDGET(view->ping_all_button), "clicked", G_CALLBACK(on_run_ping_all_equipments_clicked), view);

  gtk_grid_attach(GTK_GRID(container), GTK_WIDGET(view->stack), 0, 1, 2, 1);
  gtk_grid_attach(GTK_GRID(container), GTK_WIDGET(view->count_field.container), 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(container), GTK_WIDGET(view->timeout_field.container), 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(container), GTK_WIDGET(view->packet_size_field.container), 0, 3, 2, 1);
  gtk_grid_attach(GTK_GRID(container), GTK_WIDGET(view->run_button), 0, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(container), GTK_WIDGET(view->ping_all_button), 1, 4, 1, 1);

  return container;
}

 static GtkWidget *build_search_target(ping_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);

  GtkWidget *label = gtk_label_new("TARGET (ID / IP / MAC)");
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_add_css_class(label, "unit-field-label");

  GtkWidget *search_entry = gtk_search_entry_new();
  gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search_entry), "Search equipments...");
  gtk_widget_set_size_request(search_entry, 336, 40);
  gtk_widget_add_css_class(search_entry, "search-target");
  g_signal_connect(search_entry, "search-changed", G_CALLBACK(on_search_equipment_activated), view);

  view->list = GTK_LIST_BOX(gtk_list_box_new());
  gtk_widget_set_visible(GTK_WIDGET(view->list), FALSE);
  g_signal_connect(GTK_WIDGET(view->list), "row-selected", G_CALLBACK(on_equipment_row_selected), view);

  gtk_box_append(GTK_BOX(container), label);
  gtk_box_append(GTK_BOX(container), search_entry);
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->list));

  return container;
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

static GtkWidget *build_equipment_cell(equipment_t equipment) 
{
  const char *icons[] = {
    "assets/status-failed.svg",
    "assets/status-maintenance.svg",
    "assets/status-operational.svg",
    "assets/status-disabled.svg"
  };

  return list_item_new(equipment.name, equipment.ip_address, icons[equipment.status]);
}

void ping_view_set_actions_enabled(ping_view_t *view, bool is_active)
{
  gtk_widget_set_sensitive(GTK_WIDGET(view->run_button), is_active);
  gtk_widget_set_sensitive(GTK_WIDGET(view->ping_all_button), is_active);
}

void ping_view_update_list(ping_view_t *view, equipment_t *equipments, int count)
{
  list_remove_rows(GTK_WIDGET(view->list));

  gtk_widget_set_visible(GTK_WIDGET(view->list), TRUE);

  if (equipments == NULL || count == 0) return;

  for (int i = 0; i < count; i++)
    build_list_row(GTK_WIDGET(view->list), equipments[i]);
}

void ping_view_update_cards(ping_view_t *view)
{
  ping_stats_t stats = {0};

  ping_controller_get_stats(view->controller, &stats);

  info_card_set_value(&view->status_card, stats.status);
  info_card_set_subtitle(&view->status_card, view->controller->ip);

  info_card_set_value(&view->packets_card, stats.loss_value);
  info_card_set_subtitle(&view->packets_card, stats.loss_subtitle);

  info_card_set_value(&view->average_card, stats.latency);
  info_card_set_subtitle(&view->average_card, "from current execution");

  info_card_set_value(&view->execution_card, stats.execution_value);
  info_card_set_subtitle(&view->execution_card, stats.execution_subtitle);
}

static void on_source_selector_clicked(GtkButton *button, gpointer data)
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

  ping_controller_set_source_selection(view->controller, source);
}

static void on_search_equipment_activated(GtkSearchEntry *search, gpointer data)
{
  ping_view_t *view = (ping_view_t *)data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(search));

  ping_controller_search_equipment(view->controller, text);
}

static void on_equipment_row_selected(GtkListBox *list, GtkListBoxRow *row, gpointer data)
{
  (void)list; // unused variable
  
  ping_view_t *view = (ping_view_t *)data;
  
  if (row == NULL) return;

  const char *ip_address = g_object_get_data(G_OBJECT(row), DATA_IP_ADDRESS);

  ping_controller_set_ip_address(view->controller, ip_address);
}

static void on_run_ping_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused

  ping_view_t *view = (ping_view_t *)data; 

  //terminal_panel_clear_output(&view->terminal);
  
  ping_view_set_actions_enabled(view, FALSE);

  const char *ip_text = gtk_editable_get_text(GTK_EDITABLE(view->manual_ip_field.entry));
  const char *count = gtk_editable_get_text(GTK_EDITABLE(view->count_field.entry));
  const char *timeout = gtk_editable_get_text(GTK_EDITABLE(view->timeout_field.entry));
  const char *packet_size = gtk_editable_get_text(GTK_EDITABLE(view->packet_size_field.entry));

  ping_controller_set_ip_from_source(view->controller, ip_text);

  gtk_widget_remove_css_class(GTK_WIDGET(view->manual_ip_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->count_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->timeout_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->packet_size_field.container), "field-error");
 
  ping_validation_t error = ping_controller_validate_fields(view->controller->ip, count, timeout, packet_size);

  switch (error) 
  {
    case PING_INVALID_IP_ADDRESS:
      gtk_widget_add_css_class(GTK_WIDGET(view->manual_ip_field.container), "field-error");
      break;
    case PING_INVALID_COUNT:
      gtk_widget_add_css_class(GTK_WIDGET(view->count_field.container), "field-error");
      break;
    case PING_INVALID_TIMEOUT:
      gtk_widget_add_css_class(GTK_WIDGET(view->timeout_field.container), "field-error");
      break;
    case PING_INVALID_PACKET_SIZE:
      gtk_widget_add_css_class(GTK_WIDGET(view->packet_size_field.container), "field-error");
      break;
    case PING_OK:
      ping_controller_run_ping(view->controller, count, timeout, packet_size);
      return;
  }

  ping_view_set_actions_enabled(view, TRUE);
}

static void on_run_ping_all_equipments_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused 

  ping_view_t *view = (ping_view_t *)data; 

  ping_view_set_actions_enabled(view, FALSE);
  
  //terminal_panel_clear_output(&view->terminal);
  
  ping_controller_run_ping_all_equipments(view->controller);
}

static void on_save_output_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  ping_view_t *view = (ping_view_t *)data;

  GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(view->container)));

  GtkFileDialog *dialog = gtk_file_dialog_new();
  gtk_file_dialog_set_modal(dialog, TRUE);

  gtk_file_dialog_save(dialog, window, NULL, on_save_output_finish, view);

  g_object_unref(dialog);
}

static void on_copy_output_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  ping_view_t *view = (ping_view_t *)data;

  GdkClipboard *clipboard = gtk_widget_get_clipboard(GTK_WIDGET(view->terminal.text_view));

  char *text = terminal_panel_get_output(&view->terminal);
  if (text == NULL) return;

  gdk_clipboard_set_text(clipboard, text);

  g_free(text);
}

static void on_save_output_finish(GObject *self, GAsyncResult *res, gpointer data)
{
  ping_view_t *view = (ping_view_t *)data;

  GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(self), res, NULL);
  if (file == NULL) return;

  char *filepath = g_file_get_path(file);

  ping_controller_save_file(view->controller, filepath);

  ping_view_set_actions_enabled(view, TRUE);

  g_free(filepath);
  g_object_unref(file);
}
