#include "equipment_view.h"

#include "utils.h"
#include "macros.h"
#include "pagination.h"

#include "action_button.h"
#include "pagination_bar.h"
#include "stats_card.h"
#include "status_badge.h"
#include "summary_item.h"
#include "input_field.h"
#include "dropdown_field.h"
#include "dialog.h"
#include "alert_icon.h"
#include "table_header.h"
#include "table_cell.h"
#include "table_checkbox.h"
#include "text_label.h"
#include "status_badge.h"
#include "widget_utils.h"

// TODO: look for better way
static const char* const equipment_status[] = { "Failed", "Maintenance", "Operational", "Disabled", NULL };
static const char* const equipment_types[] = { "Router", "Firewall", "Switch", "Access Point", "Server", "NAS", "UPS", "IP Camera", "Printer", "Other", NULL };
static const char* const filter_status[] = { "All", "Failed", "Maintenance", "Operational", "Disabled", NULL };
static const char* const filter_sort[] = { "Sort", "Status", "Type", "Location", NULL};
static const char* const filter_types[] = { "All", "Router", "Firewall", "Switch", "Access Point", "Server", "NAS", "UPS", "IP Camera", "Printer", "Other", NULL };
static const char* const headers[] = { "ID", "NAME", "TYPE", "VENDOR", "MODEL", "IP ADDRESS", "MAC ADDRESS", "LOCATION", "STATUS", "LAST CHECK" };
static const int widths[] = { CELL_ID_WIDTH, CELL_NAME_WIDTH, CELL_TYPE_WIDTH, CELL_VENDOR_WIDTH, CELL_MODEL_WIDTH, CELL_IP_ADDRESS_WIDTH, CELL_MAC_ADDRESS_WIDTH, CELL_LOCATION_WIDTH, CELL_STATUS_WIDTH, CELL_LAST_CHECK_WIDTH };

static const char* DATA_EQUIPMENT = "equipment-id";

static const int INVENTORY_HEADER_COLUMN_COUNT = 10;
static const int INVENTORY_TABLE_COLUMN_COUNT = 11;

static GtkWidget *build_sidebar(equipment_view_t *view);
static GtkWidget *build_content(equipment_view_t *view);
static GtkWidget *build_header(equipment_view_t *view);
static GtkWidget *build_stats_cards(equipment_view_t *view);
static GtkWidget *build_filter_bar(equipment_view_t *view);
static GtkWidget *build_table(equipment_view_t *view);
static GtkWidget *build_pagination_bar(equipment_view_t *view);

static void build_table_header(GtkWidget *table);
static void build_table_row(equipment_view_t *view, equipment_t equipment, int row);
static GtkWidget *build_summary_card(equipment_t equipment);
static GtkWidget *build_status_cell(equipment_status_t status);

static GtkWidget *build_form(equipment_view_t *view);
static GtkWidget *build_remove_dialog(equipment_t equipment);
static void update_form_field(equipment_form_t *form, equipment_t equipment);

static void equipment_view_apply_filters(equipment_view_t *view);

// Callbacks
static void on_add_equipment_clicked(GtkButton *button, gpointer data);
static void on_edit_equipment_clicked(GtkButton *button, gpointer data);
static void on_remove_equipment_clicked(GtkButton *button, gpointer data);

static void on_add_form_submit(GtkButton *button, gpointer data);
static void on_edit_form_submit(GtkButton *button, gpointer data);
static void on_remove_dialog_submit(GtkButton *button, gpointer data);

static void on_search_entry_changed(GtkSearchEntry *search, gpointer data);
static void on_filter_dropdown_changed(GObject *self, GParamSpec *pspec, gpointer data);
static void on_row_selection_toggled(GtkCheckButton *button, gpointer data);

static void on_previous_page_clicked(GtkButton *button, gpointer data);
static void on_next_page_clicked(GtkButton *button, gpointer data);
static void on_page_clicked(GtkButton *button, gpointer data);


GtkBox *equipment_view_create(equipment_view_t *view, equipment_controller_t *controller)
{
  view->controller = controller;

  view->container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

  GtkWidget *side_bar = build_sidebar(view);
  GtkWidget *content = build_content(view);

  gtk_box_append(view->container, side_bar);
  gtk_box_append(view->container, content);

  return view->container;
}

void equipment_view_refresh(equipment_view_t *view)
{
  equipment_controller_refresh_page(view->controller);
}

void equipment_view_update_header(equipment_view_t *view)
{
  gtk_widget_set_visible(GTK_WIDGET(view->edit_button), view->controller->selected_count != 0);
  gtk_widget_set_visible(GTK_WIDGET(view->remove_button), view->controller->selected_count != 0);
}

void equipment_view_update_stats_cards(equipment_view_t *view)
{
  widget_remove_children(GTK_WIDGET(view->cards));

  equipment_stats_t stats = {0};

  equipment_controller_get_stats(view->controller, &stats);

  GtkWidget *total_card = stats_card_new("Total Equipments", stats.total, "default-card");
  GtkWidget *operational_card = stats_card_new("Operational", stats.operational, "operational-card");
  GtkWidget *failed_card = stats_card_new("Failed", stats.failed, "failed-card");
  GtkWidget *maintenance_card = stats_card_new("Maintenance", stats.maintenance, "maintenance-card");

  gtk_box_append(view->cards, total_card);
  gtk_box_append(view->cards, operational_card);
  gtk_box_append(view->cards, failed_card);
  gtk_box_append(view->cards, maintenance_card);
}

void equipment_view_update_table(equipment_view_t *view, const equipment_t *equipments, int count)
{
  table_remove_rows(GTK_WIDGET(view->table));

  if (equipments == NULL || count == 0) return;

  for (int i = 0; i < count; i++) 
  {
    build_table_row(view, equipments[i], i + 1);
  }

  GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(view->pagination_bar));
  
  if (parent != NULL)
  {
    gtk_box_remove(GTK_BOX(parent), GTK_WIDGET(view->pagination_bar));

    view->pagination_bar = GTK_BOX(build_pagination_bar(view));
    gtk_box_append(GTK_BOX(parent), GTK_WIDGET(view->pagination_bar));
  }
}

// TODO
static GtkWidget *build_sidebar(equipment_view_t *view)
{
  (void)view; // unused parameter

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 280, -1);

  return box;
}

static GtkWidget *build_content(equipment_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_set_margin_start(box, 24);
  gtk_widget_set_margin_end(box, 24);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);
  //gtk_widget_set_size_request(box, 1160, -1);

  GtkWidget *header = build_header(view);
  view->cards = GTK_BOX(build_stats_cards(view));
  GtkWidget *filters = build_filter_bar(view);
  GtkWidget *table = build_table(view);

  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->cards));
  gtk_box_append(GTK_BOX(box), filters);
  gtk_box_append(GTK_BOX(box), table);

  return box;
}

static GtkWidget *build_header(equipment_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

  GtkWidget *title = gtk_label_new("Equipment Inventory");
  gtk_widget_add_css_class(title, "inventory-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  view->remove_button = GTK_BUTTON(action_button_new("Remove", "assets/icon-remove.svg", "remove-button"));
  gtk_widget_set_visible(GTK_WIDGET(view->remove_button), FALSE);
  g_signal_connect(GTK_WIDGET(view->remove_button), "clicked", G_CALLBACK(on_remove_equipment_clicked), view);

  view->edit_button = GTK_BUTTON(action_button_new("Edit", "assets/icon-edit.svg", "edit-button"));
  gtk_widget_set_visible(GTK_WIDGET(view->edit_button), FALSE);
  g_signal_connect(GTK_WIDGET(view->edit_button), "clicked", G_CALLBACK(on_edit_equipment_clicked), view);

  view->add_button = GTK_BUTTON(action_button_new("Add Equipment", "assets/icon-add.svg", "secondary-button"));
  g_signal_connect(GTK_WIDGET(view->add_button), "clicked", G_CALLBACK(on_add_equipment_clicked), view);

  gtk_box_append(GTK_BOX(box), title);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->remove_button));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->edit_button));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->add_button));

  return box;
}

static GtkWidget *build_stats_cards(equipment_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(box, TRUE);

  equipment_stats_t stats = {0};

  equipment_controller_get_stats(view->controller, &stats);

  GtkWidget *total_card = stats_card_new("Total Equipments", stats.total, "default-card");
  GtkWidget *operational_card = stats_card_new("Operational", stats.operational, "operational-card");
  GtkWidget *failed_card = stats_card_new("Failed", stats.failed, "failed-card");
  GtkWidget *maintenance_card = stats_card_new("Maintenance", stats.maintenance, "maintenance-card");

  gtk_box_append(GTK_BOX(box), total_card);
  gtk_box_append(GTK_BOX(box), operational_card);
  gtk_box_append(GTK_BOX(box), failed_card);
  gtk_box_append(GTK_BOX(box), maintenance_card);

  return box;
}

static GtkWidget *build_filter_bar(equipment_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(box, TRUE);

  GtkWidget *search = gtk_search_entry_new();
  gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search), "Search by ID, IP or MAC...");
  gtk_widget_add_css_class(search, "inventory-search");
  gtk_widget_set_hexpand(search, TRUE);
  g_signal_connect(search, "search-changed", G_CALLBACK(on_search_entry_changed), view);

  view->sort_filter = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(filter_sort));
  gtk_widget_add_css_class(GTK_WIDGET(view->sort_filter), "inventory-filter");
  g_signal_connect(GTK_WIDGET(view->sort_filter), "notify::selected", G_CALLBACK(on_filter_dropdown_changed), view);

  view->type_filter = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(filter_types));
  gtk_widget_add_css_class(GTK_WIDGET(view->type_filter), "inventory-filter");
  g_signal_connect(GTK_WIDGET(view->type_filter), "notify::selected", G_CALLBACK(on_filter_dropdown_changed), view);

  view->status_filter = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(filter_status));
  gtk_widget_add_css_class(GTK_WIDGET(view->status_filter), "inventory-filter");
  g_signal_connect(GTK_WIDGET(view->status_filter), "notify::selected", G_CALLBACK(on_filter_dropdown_changed), view);

  gtk_box_append(GTK_BOX(box), search);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->sort_filter));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->type_filter));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->status_filter));

  return box;
}

// TODO: Not finished yet.
static GtkWidget *build_table(equipment_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_add_css_class(box, "inventory");

  GtkWidget *scrolled_window = gtk_scrolled_window_new();
  gtk_widget_set_size_request(scrolled_window, -1, 456);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
  gtk_widget_add_css_class(scrolled_window, "table-scroll");

  view->table = GTK_GRID(gtk_grid_new());
  gtk_widget_set_hexpand(GTK_WIDGET(view->table), FALSE);
  gtk_widget_set_halign(GTK_WIDGET(view->table), GTK_ALIGN_FILL);
  gtk_widget_add_css_class(GTK_WIDGET(view->table), "table");

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(view->table));

  build_table_header(GTK_WIDGET(view->table));

  view->pagination_bar = GTK_BOX(build_pagination_bar(view));

  gtk_box_append(GTK_BOX(box), scrolled_window);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->pagination_bar));

  equipment_controller_refresh_page(view->controller);

  return box;
}

static GtkWidget *build_pagination_bar(equipment_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  gtk_widget_set_size_request(box, -1, 64);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_set_halign(box, GTK_ALIGN_END);

  GtkWidget *previous_button = action_button_new(NULL, "assets/left-arrow.svg", "arrow-page");
  gtk_widget_set_margin_top(previous_button, 16);
  gtk_widget_set_margin_bottom(previous_button, 16);
  gtk_widget_set_size_request(previous_button, 32, 32);
  
  g_signal_connect(previous_button, "clicked", G_CALLBACK(on_previous_page_clicked), view);
  gtk_box_append(GTK_BOX(box), previous_button);

  int start, end;

  pagination_get_range(view->controller->pagination, &start, &end);

  for (int i = start; i <= end; i++) 
  {
    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%d", i + 1);

    GtkWidget *button = pagination_button_new(view->controller->pagination, buffer, i);
    g_signal_connect(button, "clicked", G_CALLBACK(on_page_clicked), view);

    gtk_box_append(GTK_BOX(box), button);
  }

  GtkWidget *next_button = action_button_new(NULL, "assets/right-arrow.svg", "arrow-page");
  gtk_widget_set_margin_top(next_button, 16);
  gtk_widget_set_margin_bottom(next_button, 16);
  gtk_widget_set_margin_end(next_button, 24);
  g_signal_connect(next_button, "clicked", G_CALLBACK(on_next_page_clicked), view);
  gtk_widget_set_size_request(next_button, 32, 32);
 
  gtk_box_append(GTK_BOX(box), next_button);

  return box;
}

static void build_table_header(GtkWidget *table)
{
  GtkWidget *select_all_button = gtk_check_button_new();
  gtk_widget_add_css_class(select_all_button, "table-header-checkbox");
  gtk_widget_set_size_request(select_all_button, 60, -1);
  gtk_grid_attach(GTK_GRID(table), select_all_button, 0, 0, 1, 1);

  for (int i = 0; i < INVENTORY_HEADER_COLUMN_COUNT; i++) {
    GtkWidget *label = table_header_new(headers[i], widths[i]);
    gtk_grid_attach(GTK_GRID(table), label, i + 1, 0, 1, 1);
  }
}

static void build_table_row(equipment_view_t *view, equipment_t equipment, int row)
{
  const char *css_class = (row % 2 == 0) ? "table-row-even" : "table-row-odd";
  
  GtkWidget *check_button = table_checkbox_new();
  g_object_set_data(G_OBJECT(check_button), DATA_EQUIPMENT, GINT_TO_POINTER(equipment.id));
  g_signal_connect(check_button, "toggled", G_CALLBACK(on_row_selection_toggled), view);

  char id[ID_MAX];
  equipment_format_id(equipment.id, id);

  char datetime[DATETIME_MAX];
  format_timestamp_to_datetime(equipment.last_check, datetime);

  GtkWidget *columns[] = {
    check_button,
    table_cell_new(id, CELL_ID_WIDTH),
    table_cell_new(equipment.name, CELL_NAME_WIDTH),
    table_cell_new(equipment_type_to_string(equipment.type), CELL_TYPE_WIDTH),
    table_cell_new(equipment.vendor, CELL_VENDOR_WIDTH),
    table_cell_new(equipment.model, CELL_MODEL_WIDTH),
    table_cell_new(equipment.ip_address, CELL_IP_ADDRESS_WIDTH),
    table_cell_new(equipment.mac_address, CELL_MAC_ADDRESS_WIDTH),
    table_cell_new(equipment.location, CELL_LOCATION_WIDTH),
    build_status_cell(equipment.status),
    table_cell_new(datetime, CELL_LAST_CHECK_WIDTH)
  };

  for (int i = 0; i < INVENTORY_TABLE_COLUMN_COUNT; i++) {
    if (i == 2) gtk_widget_add_css_class(columns[i], "table-name-cell");

    gtk_widget_add_css_class(columns[i], css_class);

    gtk_grid_attach(view->table, columns[i], i, row, 1, 1);
  }
}

static GtkWidget *build_form(equipment_view_t *view)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_widget_set_size_request(grid, 672, 500);
  gtk_widget_set_margin_start(grid, 24);
  gtk_widget_set_margin_end(grid, 24);
  gtk_widget_set_margin_top(grid, 24);
  gtk_widget_set_margin_bottom(grid, 40);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 24);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 24);
  gtk_widget_add_css_class(grid, "dialog-form");

  char id[ID_MAX];
  equipment_format_id(view->controller->data->equipments.next_id, id);
  //gtk_editable_set_text(GTK_EDITABLE(view->form.id_field.entry), id);

  view->form.id_field = input_field_new("Equipment ID", id, NULL);
  gtk_widget_add_css_class(GTK_WIDGET(view->form.id_field.container), "field-entry-disabled");
  gtk_editable_set_editable(GTK_EDITABLE(view->form.id_field.entry), FALSE);

  view->form.name_field = input_field_new("Equipment Name", "Core-Switch-01", NULL);
  gtk_entry_set_max_length(view->form.name_field.entry, STRING_MAX - 1);

  view->form.type_field = dropdown_field_new("Type", equipment_types);

  view->form.vendor_field = input_field_new("Vendor", "Cisco", NULL);
  gtk_entry_set_max_length(view->form.vendor_field.entry, STRING_MAX - 1);

  view->form.model_field = input_field_new("Model", "Catalyst 9300", NULL);
  gtk_entry_set_max_length(view->form.model_field.entry, STRING_MAX - 1);

  view->form.ip_field = input_field_new("IP Address", "192.168.1.1", NULL);
  gtk_entry_set_max_length(view->form.ip_field.entry, IP_MAX - 1);

  view->form.mac_field = input_field_new("MAC Address", "00:1A:2B:3C:4D:5E", NULL);
  gtk_entry_set_max_length(view->form.mac_field.entry, MAC_MAX - 1);

  view->form.location_field = input_field_new("Location", "Data Center Rack A4", NULL);
  gtk_entry_set_max_length(view->form.location_field.entry, STRING_MAX - 1);

  view->form.status_field = dropdown_field_new("Status", equipment_status);

  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.id_field.container), 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.name_field.container), 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.type_field.container), 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.vendor_field.container), 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.model_field.container), 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.ip_field.container), 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.mac_field.container), 0, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.location_field.container), 1, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.status_field.container), 0, 4, 1, 1);

  return grid;
}

static GtkWidget *build_status_cell(equipment_status_t status)
{
  switch (status) 
  {
    case STATUS_FAILED:
      return status_badge_new(equipment_status_to_string(status), "assets/status-failed.svg", "status-failed");

    case STATUS_MAINTENANCE:
      return status_badge_new(equipment_status_to_string(status), "assets/status-maintenance.svg", "status-maintenance");

    case STATUS_OPERATIONAL:
      return status_badge_new(equipment_status_to_string(status), "assets/status-operational.svg", "status-operational");

    case STATUS_DISABLED:
      return status_badge_new(equipment_status_to_string(status), "assets/status-disabled.svg", "status-disabled");

    default:
      return NULL;
  }
}

static void equipment_view_apply_filters(equipment_view_t *view)
{
  int position_status = gtk_drop_down_get_selected(view->status_filter);
  int position_type = gtk_drop_down_get_selected(view->type_filter);

  int position_sort = gtk_drop_down_get_selected(view->sort_filter);

  equipment_controller_apply_filters(view->controller, position_sort, position_status, position_type);
}

static void update_form_field(equipment_form_t *form, equipment_t equipment)
{
  char id[ID_MAX];
  equipment_format_id(equipment.id, id);

  gtk_editable_set_text(GTK_EDITABLE(form->id_field.entry), id);
  gtk_editable_set_text(GTK_EDITABLE(form->name_field.entry), equipment.name);
  gtk_editable_set_text(GTK_EDITABLE(form->vendor_field.entry), equipment.vendor);
  gtk_editable_set_text(GTK_EDITABLE(form->model_field.entry), equipment.model);
  gtk_editable_set_text(GTK_EDITABLE(form->ip_field.entry), equipment.ip_address);
  gtk_editable_set_text(GTK_EDITABLE(form->mac_field.entry), equipment.mac_address);
  gtk_editable_set_text(GTK_EDITABLE(form->location_field.entry), equipment.location);
  gtk_drop_down_set_selected(form->type_field.dropdown, equipment.type);
  gtk_drop_down_set_selected(form->status_field.dropdown, equipment.status);
}

static GtkWidget *build_remove_dialog(equipment_t equipment)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_size_request(box, 672, 500);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);
  gtk_widget_set_margin_start(box, 24);
  gtk_widget_set_margin_end(box, 24);
  gtk_widget_add_css_class(box, "dialog-form");

  GtkWidget *image = alert_icon_new();
  GtkWidget *primary_label = text_label_new("Are you sure you want to remove this equipment?", TEXT_LABEL_TITLE);
  GtkWidget *secundary_label = text_label_new("This action cannot be undone.\nEquipment with pending incidents cannot be removed.", TEXT_LABEL_DESCRIPTION);
  GtkWidget *summary_box = build_summary_card(equipment);

  gtk_box_append(GTK_BOX(box), image);
  gtk_box_append(GTK_BOX(box), primary_label);
  gtk_box_append(GTK_BOX(box), secundary_label);
  gtk_box_append(GTK_BOX(box), summary_box);

  return box;
}

static GtkWidget *build_summary_card(equipment_t equipment)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_add_css_class(box, "equipment-summary-card");
  
  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_hexpand(left, TRUE);
  gtk_widget_set_halign(left, GTK_ALIGN_START);

  char id[ID_MAX + 4];
  snprintf(id, ID_MAX + 4, "ID: EQ-%03d", equipment.id);
  
  GtkWidget *id_label = gtk_label_new(id);
  gtk_label_set_xalign(GTK_LABEL(id_label), 0.0);
  gtk_widget_add_css_class(id_label, "equipment-summary-id");

  GtkWidget *name_label = gtk_label_new(equipment.name);
  gtk_label_set_xalign(GTK_LABEL(name_label), 0.0);
  gtk_widget_add_css_class(name_label, "equipment-summary-name");

  gtk_box_append(GTK_BOX(left), id_label);
  gtk_box_append(GTK_BOX(left), name_label);

  GtkWidget *status = build_status_cell(equipment.status);
  gtk_widget_set_halign(status, GTK_ALIGN_END);
  gtk_widget_remove_css_class(status, "table-cell");

  gtk_box_append(GTK_BOX(header), left);
  gtk_box_append(GTK_BOX(header), status);
  
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 12);

  char type[STRING_MAX];
  snprintf(type, STRING_MAX, "%s", equipment_type_to_string(equipment.type));

  GtkWidget *type_box = summary_item_new("Type:", type);
  gtk_widget_set_hexpand(type_box, TRUE);
  gtk_widget_set_halign(type_box, GTK_ALIGN_START);

  GtkWidget *ip_box = summary_item_new("IP:", equipment.ip_address);

  GtkWidget *location_box = summary_item_new("Location:", equipment.location);
  gtk_widget_set_hexpand(location_box, TRUE);
  gtk_widget_set_halign(location_box, GTK_ALIGN_START);
 
  GtkWidget *mac_box = summary_item_new("MAC:", equipment.mac_address);

  gtk_grid_attach(GTK_GRID(grid), type_box, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), ip_box, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), location_box, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), mac_box, 1, 1, 1, 1);

  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), grid);

  return box;
}

static void on_add_equipment_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  equipment_view_t *view = (equipment_view_t *) data;

  view->form.layout = build_form(view);

  GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(view->container)));

  dialog_config_t config = 
  {
      .window = GTK_WIDGET(window),
      .form = view->form.layout,
      .title = "Add Equipment",
      .action = 
      {
        .label = "Add Equipment",
        .icon = "assets/icon-add.svg",
        .css = "dialog-footer-add-button",
        .callback = G_CALLBACK(on_add_form_submit),
        .data = view
      }
  };

  view->form.mode = EQUIPMENT_FORM_ADD;

  view->form.dialog = GTK_WINDOW(dialog_new(config));

  gtk_window_present(view->form.dialog);
}

static void on_edit_equipment_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter

  equipment_view_t *view = (equipment_view_t *) data;

  if (view->controller->selected_node == NULL) return;

  view->form.layout = build_form(view);

  GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(view->container)));

  dialog_config_t config = 
  {
      .window = GTK_WIDGET(window),
      .form = view->form.layout,
      .title = "Edit Equipment",
      .action = 
      {
        .label = "Edit",
        .icon = "assets/icon-edit.svg",
        .css = "edit-button",
        .callback = G_CALLBACK(on_edit_form_submit),
        .data = view
      }
  };

  update_form_field(&view->form, view->controller->selected_node->data);
  
  view->form.mode = EQUIPMENT_FORM_EDIT;

  view->form.dialog = GTK_WINDOW(dialog_new(config));

  gtk_window_present(view->form.dialog);
}

static void on_remove_equipment_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter

  equipment_view_t *view = (equipment_view_t *) data;

  if (view->controller->selected_node == NULL) return;

  view->form.layout = build_remove_dialog(view->controller->selected_node->data);

  GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(view->container)));

  dialog_config_t config = 
  {
      .window = GTK_WIDGET(window),
      .form = view->form.layout,
      .title = "Remove Equipment",
      .action = 
      {
        .label = "Remove",
        .icon = "assets/icon-remove.svg",
        .css = "remove-button",
        .callback = G_CALLBACK(on_remove_dialog_submit),
        .data = view
      }
  };

  view->form.mode = EQUIPMENT_FORM_REMOVE;

  view->form.dialog = GTK_WINDOW(dialog_new(config));

  gtk_window_present(view->form.dialog);
}

static void on_add_form_submit(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  equipment_view_t *view = (equipment_view_t *) data;

  const char *name_text = gtk_editable_get_text(GTK_EDITABLE(view->form.name_field.entry));
  const char *vendor_text = gtk_editable_get_text(GTK_EDITABLE(view->form.vendor_field.entry));
  const char *model_text = gtk_editable_get_text(GTK_EDITABLE(view->form.model_field.entry));
  const char *ip_text = gtk_editable_get_text(GTK_EDITABLE(view->form.ip_field.entry));
  const char *mac_text = gtk_editable_get_text(GTK_EDITABLE(view->form.mac_field.entry));
  const char *location_text = gtk_editable_get_text(GTK_EDITABLE(view->form.location_field.entry));
  
  equipment_t new = {0};

  snprintf(new.name, STRING_MAX, "%s", name_text);
  snprintf(new.vendor, STRING_MAX, "%s", vendor_text);
  snprintf(new.model, STRING_MAX, "%s", model_text);
  snprintf(new.ip_address, IP_MAX, "%s", ip_text);
  snprintf(new.mac_address, MAC_MAX, "%s", mac_text);
  snprintf(new.location, STRING_MAX, "%s", location_text);

  gtk_widget_remove_css_class(GTK_WIDGET(view->form.name_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.vendor_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.model_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.ip_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.mac_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.location_field.container), "field-error");

  equipment_validation_t error = equipment_controller_validate_add(view->controller, new);

  switch (error) 
  {
    case EQUIPMENT_INVALID_NAME:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.name_field.container), "field-error");
      return;

    case EQUIPMENT_INVALID_VENDOR:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.vendor_field.container), "field-error");
      return;

    case EQUIPMENT_INVALID_MODEL:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.model_field.container), "field-error");
      return;

    case EQUIPMENT_INVALID_IP:
    case EQUIPMENT_DUPLICATE_IP:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.ip_field.container), "field-error");
      return;

    case EQUIPMENT_INVALID_MAC:
    case EQUIPMENT_DUPLICATE_MAC:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.mac_field.container), "field-error");
      return;

    case EQUIPMENT_INVALID_LOCATION:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.location_field.container), "field-error");
      return;

    case EQUIPMENT_VALID: break;
  }

  new.type = gtk_drop_down_get_selected(view->form.type_field.dropdown);
  new.status = gtk_drop_down_get_selected(view->form.status_field.dropdown);

  equipment_controller_add(view->controller, new);

  gtk_window_destroy(view->form.dialog);
}

static void on_edit_form_submit(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  equipment_view_t *view = (equipment_view_t *) data;

  const char *name_text = gtk_editable_get_text(GTK_EDITABLE(view->form.name_field.entry));
  const char *vendor_text = gtk_editable_get_text(GTK_EDITABLE(view->form.vendor_field.entry));
  const char *model_text = gtk_editable_get_text(GTK_EDITABLE(view->form.model_field.entry));
  const char *ip_text = gtk_editable_get_text(GTK_EDITABLE(view->form.ip_field.entry));
  const char *mac_text = gtk_editable_get_text(GTK_EDITABLE(view->form.mac_field.entry));
  const char *location_text = gtk_editable_get_text(GTK_EDITABLE(view->form.location_field.entry));
  
  equipment_t equipment = {0};

  snprintf(equipment.name, STRING_MAX, "%s", name_text);
  snprintf(equipment.vendor, STRING_MAX, "%s", vendor_text);
  snprintf(equipment.model, STRING_MAX, "%s", model_text);
  snprintf(equipment.ip_address, IP_MAX, "%s", ip_text);
  snprintf(equipment.mac_address, MAC_MAX, "%s", mac_text);
  snprintf(equipment.location, STRING_MAX, "%s", location_text);

  gtk_widget_remove_css_class(GTK_WIDGET(view->form.name_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.vendor_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.model_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.ip_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.mac_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.location_field.container), "field-error");

  equipment_validation_t error = equipment_controller_validate_edit(view->controller, equipment);

  switch (error) 
  {
    case EQUIPMENT_INVALID_NAME:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.name_field.container), "field-error");
      return;

    case EQUIPMENT_INVALID_VENDOR:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.vendor_field.container), "field-error");
      return;

    case EQUIPMENT_INVALID_MODEL:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.model_field.container), "field-error");
      return;

    case EQUIPMENT_INVALID_IP:
    case EQUIPMENT_DUPLICATE_IP:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.ip_field.container), "field-error");
      return;

    case EQUIPMENT_INVALID_MAC:
    case EQUIPMENT_DUPLICATE_MAC:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.mac_field.container), "field-error");
      return;

    case EQUIPMENT_INVALID_LOCATION:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.location_field.container), "field-error");
      return;

    case EQUIPMENT_VALID: break;
  }

  equipment.type = gtk_drop_down_get_selected(view->form.type_field.dropdown);
  equipment.status = gtk_drop_down_get_selected(view->form.status_field.dropdown);

  equipment_controller_edit(view->controller, equipment);

  gtk_window_destroy(view->form.dialog);
}

static void on_remove_dialog_submit(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter

  equipment_view_t *view = (equipment_view_t *) data;

  if (!equipment_controller_can_remove(view->controller)) return;

  equipment_controller_remove(view->controller);

  gtk_window_destroy(view->form.dialog);
}

static void on_search_entry_changed(GtkSearchEntry *search, gpointer data)
{
  equipment_view_t *view = (equipment_view_t *) data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(search));

  equipment_controller_search(view->controller, text);
}

static void on_filter_dropdown_changed(GObject *self, GParamSpec *pspec, gpointer data)
{
  (void)self; // unused parameter
  (void)pspec; // unused parameter

  equipment_view_t *view = (equipment_view_t *) data;

  equipment_view_apply_filters(view);
}

static void on_row_selection_toggled(GtkCheckButton *button, gpointer data)
{
  equipment_view_t *view = (equipment_view_t *) data;

  int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), DATA_EQUIPMENT));
  bool is_active = gtk_check_button_get_active(button);

  equipment_controller_handle_toggled(view->controller, id, is_active);
}

static void on_previous_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  equipment_view_t *view = (equipment_view_t *)data;

  pagination_previous(&view->controller->pagination);

  equipment_controller_update_table(view->controller);
}

static void on_next_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter 

  equipment_view_t *view = (equipment_view_t *)data;

  pagination_next(&view->controller->pagination);

  equipment_controller_update_table(view->controller);
}

static void on_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter 
  
  equipment_view_t *view = (equipment_view_t *)data;

  int page_number = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "page-number"));

  pagination_set_page_number(&view->controller->pagination, page_number);

  equipment_controller_update_table(view->controller);
}
