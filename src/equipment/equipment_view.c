#include "equipment_view.h"

#include "utils.h"
#include "macros.h"

#include "action_button.h"
#include "status_badge.h"
#include "summary_item.h"
#include "dialog.h"
#include "alert_icon.h"
#include "table_header.h"
#include "table_cell.h"
#include "table_checkbox.h"
#include "text_label.h"
#include "widget_utils.h"

// TODO: look for better way
static const char* const equipment_status[] = { "Failed", "Maintenance", "Operational", "Disabled", NULL };
static const char* const equipment_types[] = { "Router", "Firewall", "Switch", "Access Point", "Server", "NAS", "UPS", "IP Camera", "Printer", "Other", NULL };
static const char* const filter_status[] = { "All", "Failed", "Maintenance", "Operational", "Disabled", NULL };
static const char* const filter_sort[] = { "Sort", "Status", "Type", "Location", NULL};
static const char* const filter_types[] = { "All", "Router", "Firewall", "Switch", "Access Point", "Server", "NAS", "UPS", "IP Camera", "Printer", "Other", NULL };

static const char* DATA_EQUIPMENT = "EQUIPMENT-ID";

static const int INVENTORY_TABLE_COLUMN_COUNT = 10;

static GtkWidget *build_sidebar(equipment_view_t *view);
static GtkWidget *build_content(equipment_view_t *view);
static GtkWidget *build_header(equipment_view_t *view);
static GtkWidget *build_stats_cards(equipment_view_t *view);
static GtkWidget *build_filter_bar(equipment_view_t *view);

static GtkWidget *build_table(equipment_view_t *view);
static void build_table_row(equipment_view_t *view, equipment_t equipment);

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

static void on_page_clicked(void *data);


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

void equipment_view_destroy(equipment_view_t *view)
{
  table_destroy(&view->table);
}

void equipment_view_refresh(equipment_view_t *view)
{
  equipment_controller_refresh_page(view->controller);
}

void equipment_view_update_header(equipment_view_t *view)
{
  bool has_selection = view->controller->selected_count != 0;

  gtk_widget_set_visible(GTK_WIDGET(view->edit_button), has_selection);
  gtk_widget_set_visible(GTK_WIDGET(view->remove_button), has_selection);
}

void equipment_view_update_stats_cards(equipment_view_t *view)
{
  equipment_stats_t stats = {0};

  equipment_controller_get_stats(view->controller, &stats);

  stats_card_set_value(&view->total_card, stats.total);
  stats_card_set_value(&view->operational_card, stats.operational);
  stats_card_set_value(&view->failed_card, stats.failed);
  stats_card_set_value(&view->maintenance_card, stats.maintenance);
}

void equipment_view_update_table(equipment_view_t *view, const equipment_t *equipments, int count)
{
  table_clear_rows(&view->table);

  if (equipments == NULL || count == 0) return;

  table_refresh(&view->table);
  for (int i = 0; i < count; i++) 
    build_table_row(view, equipments[i]);

  table_refresh(&view->table);
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
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_hexpand(container, TRUE);
  gtk_widget_set_margin_start(container, 24);
  gtk_widget_set_margin_end(container, 24);
  gtk_widget_set_margin_top(container, 24);
  gtk_widget_set_margin_bottom(container, 24);
  //gtk_widget_set_size_request(container, 1160, -1);

  GtkWidget *header = build_header(view);
  GtkWidget *cards = build_stats_cards(view);
  GtkWidget *filters = build_filter_bar(view);
  GtkWidget *table = build_table(view);

  gtk_box_append(GTK_BOX(container), header);
  gtk_box_append(GTK_BOX(container), cards);
  gtk_box_append(GTK_BOX(container), filters);
  gtk_box_append(GTK_BOX(container), table);

  return container;
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
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(container, TRUE);

  equipment_stats_t stats = {0};

  equipment_controller_get_stats(view->controller, &stats);

  view->total_card = stats_card_new("Total Equipments", stats.total, "default-card");
  view->operational_card = stats_card_new("Operational", stats.operational, "operational-card");
  view->failed_card = stats_card_new("Failed", stats.failed, "failed-card");
  view->maintenance_card = stats_card_new("Maintenance", stats.maintenance, "maintenance-card");

  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->total_card.container));
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->operational_card.container));
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->failed_card.container));
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->maintenance_card.container));

  return container;
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
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  table_column_t columns[] = {
    { "ID",          CELL_ID_WIDTH          },
    { "NAME",        CELL_NAME_WIDTH        },
    { "TYPE",        CELL_TYPE_WIDTH        },
    { "VENDOR",      CELL_VENDOR_WIDTH      },
    { "MODEL",       CELL_VENDOR_WIDTH      },
    { "IP ADDRESS",  CELL_IP_ADDRESS_WIDTH  },
    { "MAC ADDRESS", CELL_MAC_ADDRESS_WIDTH },
    { "LOCATION",    CELL_LOCATION_WIDTH    },
    { "STATUS",      CELL_STATUS_WIDTH      },
    { "LAST CHECK",  CELL_LAST_CHECK_WIDTH  }
  };

  view->table = table_new(columns, INVENTORY_TABLE_COLUMN_COUNT, TRUE);
  table_set_pagination(&view->table, &view->controller->pagination, on_page_clicked, view);

  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->table.container));

  equipment_controller_refresh_page(view->controller);

  return container;
}

static void build_table_row(equipment_view_t *view, equipment_t equipment)
{
  table_row_t row = table_row_new(INVENTORY_TABLE_COLUMN_COUNT);

  GtkWidget *check_button = table_checkbox_new();
  g_object_set_data(G_OBJECT(check_button), DATA_EQUIPMENT, GINT_TO_POINTER(equipment.id));
  g_signal_connect(check_button, "toggled", G_CALLBACK(on_row_selection_toggled), view);

  char id[ID_MAX];
  equipment_format_id(equipment.id, id);

  char datetime[DATETIME_MAX];
  format_timestamp_to_datetime(equipment.last_check, datetime);

  table_row_insert_cell(&row, check_button);
  table_row_insert_cell(&row, table_cell_new(id, CELL_ID_WIDTH, "table-cell"));
  table_row_insert_cell(&row, table_cell_new(equipment.name, CELL_NAME_WIDTH, "table-bold-cell"));
  table_row_insert_cell(&row, table_cell_new(equipment_type_to_string(equipment.type), CELL_TYPE_WIDTH, "table-cell"));
  table_row_insert_cell(&row, table_cell_new(equipment.vendor, CELL_VENDOR_WIDTH, "table-cell"));
  table_row_insert_cell(&row, table_cell_new(equipment.model, CELL_MODEL_WIDTH, "table-cell"));
  table_row_insert_cell(&row, table_cell_new(equipment.ip_address, CELL_IP_ADDRESS_WIDTH, "table-cell"));
  table_row_insert_cell(&row, table_cell_new(equipment.mac_address, CELL_MAC_ADDRESS_WIDTH, "table-cell"));
  table_row_insert_cell(&row, table_cell_new(equipment.location, CELL_LOCATION_WIDTH, "table-cell"));
  table_row_insert_cell(&row, build_status_cell(equipment.status));
  table_row_insert_cell(&row, table_cell_new(datetime, CELL_LAST_CHECK_WIDTH, "table-cell"));

  table_insert_row(&view->table, &row);
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
  const char *icons[] = {
    "assets/status-failed.svg",
    "assets/status-maintenance.svg",
    "assets/status-operational.svg",
    "assets/status-disabled.svg"
  };

  const char *css[] = {
    "status-failed",
    "status-maintenance",
    "status-operational",
    "status-disabled"
  };

  return status_badge_new(equipment_status_to_string(status), icons[status], css[status]);
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

static void on_page_clicked(void *data)
{
  equipment_view_t *view = (equipment_view_t *) data;

  equipment_controller_update_table(view->controller);
}
