#include "ui_inventory.h"

#include "utils.h"
#include "ui_widgets.h"
#include "macros.h"
#include "ui_macros.h"

// TODO: look for better way
static const char* const equipment_status[] = { "Failed", "Maintenance", "Operational", "Disabled", NULL };
static const char* const equipment_types[] = { "Router", "Firewall", "Switch", "Access Point", "Server", "NAS", "UPS", "IP Camera", "Printer", "Other", NULL };
static const char* const filter_status[] = { "All", "Failed", "Maintenance", "Operational", "Disabled", NULL };
static const char* const filter_types[] = { "All", "Router", "Firewall", "Switch", "Access Point", "Server", "NAS", "UPS", "IP Camera", "Printer", "Other", NULL };
static const char* const headers[] = { "ID", "NAME", "TYPE", "VENDOR", "MODEL", "IP ADDRESS", "MAC ADDRESS", "LOCATION", "STATUS", "LAST CHECK" };
static const int widths[] = { CELL_ID_WIDTH, CELL_NAME_WIDTH, CELL_TYPE_WIDTH, CELL_VENDOR_WIDTH, CELL_MODEL_WIDTH, CELL_IP_ADDRESS_WIDTH, CELL_MAC_ADDRESS_WIDTH, CELL_LOCATION_WIDTH, CELL_STATUS_WIDTH, CELL_LAST_CHECK_WIDTH };

static const char* DATA_UI_INVENTORY = "ui-inventory";
static const char* DATA_EQUIPMENT_NODE = "equipment-node";
static const char* DATA_TYPE_FILTER = "inventory-type-filter";
static const char* DATA_STATUS_FILTER = "inventory-status-filter";
static const char* DATA_REMOVE_BUTTON = "inventory-remove";
static const char* DATA_EDIT_BUTTON = "inventory-edit";
static const int INVENTORY_HEADER_COLUMN_COUNT = 10;
static const int INVENTORY_TABLE_COLUMN_COUNT = 11;

static GtkWidget *create_side_bar(application_t *application);
static GtkWidget *create_content(ui_inventory_t *ui_inventory);
static GtkWidget *create_inventory_header(ui_inventory_t *ui_inventory);
static GtkWidget *create_inventory_stats_cards(ui_inventory_t *ui_inventory);
static GtkWidget *create_inventory_filters(ui_inventory_t *ui_inventory);
static GtkWidget *create_inventory_table(ui_inventory_t *ui_inventory);
static void create_inventory_table_header(GtkWidget *grid);
static void create_inventory_table_row(GtkWidget *grid, equipment_node_t *node, int row);
static GtkWidget *create_inventory_status_cell(equipment_status_t status);
static GtkWidget *create_equipment_form(equipment_list_t *equipments);
static void refresh_inventory_table(GtkWidget *grid, equipment_list_t *equipments);
static void apply_inventory_filters(ui_inventory_t *ui_inventory);
static void update_inventory_header(ui_inventory_t *ui_inventory);
static void update_equipment_form(GtkWidget *form, equipment_t equipment);
static GtkWidget *create_remove_form(equipment_t equipment);
static GtkWidget *create_equipment_summary_card(equipment_t equipment);

// Callbacks
static void on_add_equipment_clicked(GtkButton *button, gpointer data);
static void on_edit_equipment_clicked(GtkButton *button, gpointer data);
static void on_remove_equipment_clicked(GtkButton *button, gpointer data);
static void on_add_equipment_confirmed(GtkButton *button, gpointer data);
static void on_edit_equipment_confirmed(GtkButton *button, gpointer data);
static void on_remove_equipment_confirmed(GtkButton *button, gpointer data);
static void on_equipment_search_changed(GtkSearchEntry *search, gpointer data);
static void on_inventory_filter_changed(GObject *self, GParamSpec *pspec, gpointer data);
static void on_equipment_check_button_toggled(GtkCheckButton *button, gpointer data);


GtkWidget *create_page_inventory(ui_t *ui)
{
  ui_inventory_t *ui_inventory = malloc(sizeof(ui_inventory_t));
  if (ui_inventory == NULL) return NULL;

  ui_inventory->application = ui->application;
  ui_inventory->window = ui->window;
  ui_inventory->selected_count = 0;
  ui_inventory->selected_node = NULL;

  ui_inventory->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  GtkWidget *side_bar = create_side_bar(ui_inventory->application);
  GtkWidget *content = create_content(ui_inventory);

  gtk_box_append(GTK_BOX(ui_inventory->container), side_bar);
  gtk_box_append(GTK_BOX(ui_inventory->container), content);

  g_object_set_data_full(G_OBJECT(ui_inventory->container), "ui_inventory", ui_inventory, free); // ownership + free

  return ui_inventory->container;
}

// TODO
static GtkWidget *create_side_bar(application_t *application)
{
  (void)application; // unused parameter

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 280, -1);

  return box;
}

static GtkWidget *create_content(ui_inventory_t *ui_inventory)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_set_margin_start(box, 24);
  gtk_widget_set_margin_end(box, 24);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);
  //gtk_widget_set_size_request(box, 1160, -1);

  GtkWidget *header = create_inventory_header(ui_inventory);
  GtkWidget *stats_cards = create_inventory_stats_cards(ui_inventory);
  GtkWidget *filters = create_inventory_filters(ui_inventory);
  GtkWidget *table = create_inventory_table(ui_inventory);

  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), stats_cards);
  gtk_box_append(GTK_BOX(box), filters);
  gtk_box_append(GTK_BOX(box), table);

  return box;
}

static GtkWidget *create_inventory_header(ui_inventory_t *ui_inventory)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

  GtkWidget *title = gtk_label_new("Equipment Inventory");
  gtk_widget_add_css_class(title, "inventory-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  GtkWidget *add_equipment_button = create_secondary_button("Add Equipment", "assets/icon-add.svg", "secondary-button");
  g_signal_connect(add_equipment_button, "clicked", G_CALLBACK(on_add_equipment_clicked), ui_inventory);

  GtkWidget *remove_button = create_secondary_button("Remove", "assets/icon-remove.svg", "remove-button");
  gtk_widget_set_visible(remove_button, FALSE);
  g_object_set_data(G_OBJECT(ui_inventory->container), DATA_REMOVE_BUTTON, remove_button);
  g_signal_connect(remove_button, "clicked", G_CALLBACK(on_remove_equipment_clicked), ui_inventory);

  GtkWidget *edit_button = create_secondary_button("Edit", "assets/icon-edit.svg", "edit-button");
  gtk_widget_set_visible(edit_button, FALSE);
  g_object_set_data(G_OBJECT(ui_inventory->container), DATA_EDIT_BUTTON, edit_button);
  g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_equipment_clicked), ui_inventory);

  gtk_box_append(GTK_BOX(box), title);
  gtk_box_append(GTK_BOX(box), edit_button);
  gtk_box_append(GTK_BOX(box), remove_button);
  gtk_box_append(GTK_BOX(box), add_equipment_button);

  return box;
}

static GtkWidget *create_inventory_stats_cards(ui_inventory_t *ui_inventory)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(box, TRUE);

  GtkWidget *total_card = create_stats_card("Total Equipments", "4", "default-card");
  GtkWidget *operational_card = create_stats_card("Operational", "2", "operational-card");
  GtkWidget *failed_card = create_stats_card("Failed", "1", "failed-card");
  GtkWidget *maintenance_card = create_stats_card("Maintenance", "1", "maintenance-card");

  gtk_box_append(GTK_BOX(box), total_card);
  gtk_box_append(GTK_BOX(box), operational_card);
  gtk_box_append(GTK_BOX(box), failed_card);
  gtk_box_append(GTK_BOX(box), maintenance_card);

  return box;
}

static GtkWidget *create_inventory_filters(ui_inventory_t *ui_inventory)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(box, TRUE);

  GtkWidget *search = gtk_search_entry_new();
  gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search), "Search by ID, IP or MAC...");
  gtk_widget_add_css_class(search, "inventory-search");
  gtk_widget_set_hexpand(search, TRUE);
  g_signal_connect(search, "search-changed", G_CALLBACK(on_equipment_search_changed), ui_inventory);

  GtkWidget *dropdown_type = gtk_drop_down_new_from_strings(filter_types);
  gtk_widget_add_css_class(dropdown_type, "inventory-filter");
  g_signal_connect(dropdown_type, "notify::selected", G_CALLBACK(on_inventory_filter_changed), ui_inventory);
  g_object_set_data(G_OBJECT(ui_inventory->container), DATA_TYPE_FILTER, dropdown_type);

  GtkWidget *dropdown_status = gtk_drop_down_new_from_strings(filter_status);
  gtk_widget_add_css_class(dropdown_status, "inventory-filter");
  g_signal_connect(dropdown_status, "notify::selected", G_CALLBACK(on_inventory_filter_changed), ui_inventory);
  g_object_set_data(G_OBJECT(ui_inventory->container), DATA_STATUS_FILTER, dropdown_status);

  gtk_box_append(GTK_BOX(box), search);
  gtk_box_append(GTK_BOX(box), dropdown_type);
  gtk_box_append(GTK_BOX(box), dropdown_status);

  return box;
}

// TODO: Not finished yet.
static GtkWidget *create_inventory_table(ui_inventory_t *ui_inventory)
{
  GtkWidget *scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
  gtk_widget_add_css_class(scrolled_window, "table-scroll");

  ui_inventory->table = gtk_grid_new();
  gtk_widget_set_hexpand(ui_inventory->table, FALSE);
  gtk_widget_set_halign(ui_inventory->table, GTK_ALIGN_FILL);
  gtk_widget_add_css_class(ui_inventory->table, "table");

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), ui_inventory->table);

  g_object_set_data(G_OBJECT(ui_inventory->table), DATA_UI_INVENTORY, ui_inventory);

  create_inventory_table_header(ui_inventory->table);

  equipment_node_t *node = ui_inventory->application->equipments.head;
  if (node == NULL) return scrolled_window;

  int row = 1;

  while (node != NULL)
  {
    create_inventory_table_row(ui_inventory->table, node, row);
    node = node->next;
    row++;
  }

  return scrolled_window;
}

static void create_inventory_table_header(GtkWidget *grid)
{
  GtkWidget *select_all_button = gtk_check_button_new();
  gtk_widget_add_css_class(select_all_button, "table-header-checkbox");
  gtk_widget_set_size_request(select_all_button, 60, -1);
  gtk_grid_attach(GTK_GRID(grid), select_all_button, 0, 0, 1, 1);

  for (int i = 0; i < INVENTORY_HEADER_COLUMN_COUNT; i++) {
    GtkWidget *label = create_table_header(headers[i], widths[i]);
    gtk_grid_attach(GTK_GRID(grid), label, i + 1, 0, 1, 1);
  }
}

static void create_inventory_table_row(GtkWidget *grid, equipment_node_t *node, int row)
{
  ui_inventory_t *ui_inventory = g_object_get_data(G_OBJECT(grid), DATA_UI_INVENTORY);

  equipment_t equipment = node->data;

  const char *css_class = (row % 2 == 0) ? "table-row-even" : "table-row-odd";
  
  GtkWidget *check_button = create_table_checkbox();
  g_object_set_data(G_OBJECT(check_button), DATA_EQUIPMENT_NODE, (void *)node);
  g_signal_connect(check_button, "toggled", G_CALLBACK(on_equipment_check_button_toggled), ui_inventory);

  char id[ID_MAX];
  snprintf(id, ID_MAX, "EQ-%03d", equipment.id);

  char datetime[DATETIME_MAX];
  get_datetime(equipment.last_check, datetime);

  GtkWidget *columns[] = {
    check_button,
    create_table_cell(id, CELL_ID_WIDTH),
    create_table_cell(equipment.name, CELL_NAME_WIDTH),
    create_table_cell(equipment_type_to_string(equipment.type), CELL_TYPE_WIDTH),
    create_table_cell(equipment.vendor, CELL_VENDOR_WIDTH),
    create_table_cell(equipment.model, CELL_MODEL_WIDTH),
    create_table_cell(equipment.ip_address, CELL_IP_ADDRESS_WIDTH),
    create_table_cell(equipment.mac_address, CELL_MAC_ADDRESS_WIDTH),
    create_table_cell(equipment.location, CELL_LOCATION_WIDTH),
    create_inventory_status_cell(equipment.status),
    create_table_cell(datetime, CELL_LAST_CHECK_WIDTH)
  };

  for (int i = 0; i < INVENTORY_TABLE_COLUMN_COUNT; i++) {
    if (i == 2) gtk_widget_add_css_class(columns[i], "table-name-cell");

    gtk_widget_add_css_class(columns[i], css_class);

    gtk_grid_attach(GTK_GRID(grid), columns[i], i, row, 1, 1);
  }
}

static GtkWidget *create_inventory_status_cell(equipment_status_t status)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "table-cell");
  gtk_widget_add_css_class(box, "status-cell");

  GtkWidget *border, *label, *image;

  border = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_add_css_class(border, "status-badge");

  label = gtk_label_new(equipment_status_to_string(status));
  gtk_widget_add_css_class(label, "status-label");

  switch (status) 
  {
    case STATUS_FAILED:
      image = gtk_image_new_from_file("assets/status-failed.svg");
      gtk_widget_add_css_class(border, "status-failed");
      break;
    case STATUS_MAINTENANCE:
      image = gtk_image_new_from_file("assets/status-maintenance.svg");
      gtk_widget_add_css_class(border, "status-maintenance");
      break;
    case STATUS_OPERATIONAL:
      image = gtk_image_new_from_file("assets/status-operational.svg");
      gtk_widget_add_css_class(border, "status-operational");
      break;
    case STATUS_DISABLED:
      image = gtk_image_new_from_file("assets/status-disabled.svg");
      gtk_widget_add_css_class(border, "status-disabled");
      break;
  }

  gtk_image_set_pixel_size(GTK_IMAGE(image), 6);

  gtk_box_append(GTK_BOX(box), border);
  gtk_box_append(GTK_BOX(border), image);
  gtk_box_append(GTK_BOX(border), label);

  return box;
}

static GtkWidget *create_equipment_form(equipment_list_t *equipments)
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

  GtkWidget *entry_id = create_text_field(grid, "Equipment ID", NULL, 0, 0);
  gtk_widget_add_css_class(entry_id, "form-entry-disabled");
  gtk_editable_set_editable(GTK_EDITABLE(entry_id), FALSE);
  g_object_set_data(G_OBJECT(grid), "entry-id", entry_id);

  char id[ID_MAX];
  snprintf(id, ID_MAX, "EQ-%03d", equipments->next_id);
  gtk_editable_set_text(GTK_EDITABLE(entry_id), id);

  GtkWidget *entry_name = create_text_field(grid, "Equipment Name", "Core-Switch-01", 0, 1);
  gtk_entry_set_max_length(GTK_ENTRY(entry_name), STRING_MAX - 1);
  g_object_set_data(G_OBJECT(grid), "entry-name", entry_name);

  GtkWidget *dropdown_type = create_dropdown_field(grid, "Type", equipment_types, 1, 0);
  g_object_set_data(G_OBJECT(grid), "dropdown-type", dropdown_type);

  GtkWidget *entry_vendor = create_text_field(grid, "Vendor", "Cisco", 1, 1);
  gtk_entry_set_max_length(GTK_ENTRY(entry_vendor), STRING_MAX - 1);
  g_object_set_data(G_OBJECT(grid), "entry-vendor", entry_vendor);

  GtkWidget *entry_model = create_text_field(grid, "Model", "Catalyst 9300", 2, 0);
  gtk_entry_set_max_length(GTK_ENTRY(entry_model), STRING_MAX - 1);
  g_object_set_data(G_OBJECT(grid), "entry-model", entry_model);

  GtkWidget *entry_ip = create_text_field(grid, "IP Address", "192.168.1.1", 2, 1);
  gtk_entry_set_max_length(GTK_ENTRY(entry_ip), IP_MAX - 1);
  g_object_set_data(G_OBJECT(grid), "entry-ip", entry_ip);

  GtkWidget *entry_mac = create_text_field(grid, "MAC Address", "00:1A:2B:3C:4D:5E", 3, 0); 
  gtk_entry_set_max_length(GTK_ENTRY(entry_mac), MAC_MAX - 1);
  g_object_set_data(G_OBJECT(grid), "entry-mac", entry_mac);

  GtkWidget *entry_location = create_text_field(grid, "Location", "Data Center Rack A4", 3, 1);
  gtk_entry_set_max_length(GTK_ENTRY(entry_location), STRING_MAX - 1);
  g_object_set_data(G_OBJECT(grid), "entry-location", entry_location);

  GtkWidget *dropdown_status = create_dropdown_field(grid, "Status", equipment_status, 4, 0);
  g_object_set_data(G_OBJECT(grid), "dropdown-status", dropdown_status);

  return grid;
}

static void refresh_inventory_table(GtkWidget *grid, equipment_list_t *equipments)
{
  remove_table_rows(grid);

  equipment_node_t *node = equipments->head;
  if (node == NULL) return;

  int row = 1;

  while (node != NULL)
  {
    create_inventory_table_row(grid, node, row);
    node = node->next;
    row++;
  }
}

static void apply_inventory_filters(ui_inventory_t *ui_inventory)
{
  GtkWidget *dropdown_status = g_object_get_data(G_OBJECT(ui_inventory->container), DATA_STATUS_FILTER);
  GtkWidget *dropdown_type = g_object_get_data(G_OBJECT(ui_inventory->container), DATA_TYPE_FILTER);

  int position_status = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_status));
  int position_type = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_type));

  if (position_status == 0 && position_type == 0) 
  {
    refresh_inventory_table(ui_inventory->table, &ui_inventory->application->equipments);
    return;
  }

  equipment_list_t filtered; // Temporary list
  equipment_list_init(&filtered);

  if (position_status == 0 && position_type != 0) 
    equipment_filter_by_type(&ui_inventory->application->equipments, position_type - 1, &filtered);

  else if (position_type == 0 && position_status != 0)
    equipment_filter_by_status(&ui_inventory->application->equipments, position_status - 1, &filtered);

  else 
    equipment_filter(&ui_inventory->application->equipments, position_status - 1, position_type - 1, &filtered);

  refresh_inventory_table(ui_inventory->table, &filtered);

  equipment_list_destroy(&filtered);
}

static void update_inventory_header(ui_inventory_t *ui_inventory)
{
  GtkWidget *edit_button = g_object_get_data(G_OBJECT(ui_inventory->container), DATA_EDIT_BUTTON);
  GtkWidget *remove_button = g_object_get_data(G_OBJECT(ui_inventory->container), DATA_REMOVE_BUTTON);

  gtk_widget_set_visible(edit_button, ui_inventory->selected_count == 1);
  gtk_widget_set_visible(remove_button, ui_inventory->selected_count == 1);
}

static void update_equipment_form(GtkWidget *form, equipment_t equipment)
{
  char id[ID_MAX];
  snprintf(id, ID_MAX, "EQ-%03d", equipment.id);

  gtk_editable_set_text(GTK_EDITABLE(g_object_get_data(G_OBJECT(form), "entry-id")), id);
  gtk_editable_set_text(GTK_EDITABLE(g_object_get_data(G_OBJECT(form), "entry-name")), equipment.name);
  gtk_editable_set_text(GTK_EDITABLE(g_object_get_data(G_OBJECT(form), "entry-vendor")), equipment.vendor);
  gtk_editable_set_text(GTK_EDITABLE(g_object_get_data(G_OBJECT(form), "entry-model")), equipment.model);
  gtk_editable_set_text(GTK_EDITABLE(g_object_get_data(G_OBJECT(form), "entry-ip")), equipment.ip_address);
  gtk_editable_set_text(GTK_EDITABLE(g_object_get_data(G_OBJECT(form), "entry-mac")), equipment.mac_address);
  gtk_editable_set_text(GTK_EDITABLE(g_object_get_data(G_OBJECT(form), "entry-location")), equipment.location);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(g_object_get_data(G_OBJECT(form), "dropdown-type")), equipment.type);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(g_object_get_data(G_OBJECT(form), "dropdown-status")), equipment.status);
}

static GtkWidget *create_remove_form(equipment_t equipment)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_size_request(box, 672, 500);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);
  gtk_widget_set_margin_start(box, 24);
  gtk_widget_set_margin_end(box, 24);
  gtk_widget_add_css_class(box, "dialog-form");

  GtkWidget *image = create_alert_icon();
  GtkWidget *primary_label = create_remove_primary_label("Are you sure you want to remove this equipment?");
  GtkWidget *secundary_label = create_remove_secundary_label("This action cannot be undone.\nEquipment with pending incidents cannot be removed.");
  GtkWidget *summary_box = create_equipment_summary_card(equipment);

  gtk_box_append(GTK_BOX(box), image);
  gtk_box_append(GTK_BOX(box), primary_label);
  gtk_box_append(GTK_BOX(box), secundary_label);
  gtk_box_append(GTK_BOX(box), summary_box);

  return box;
}

static GtkWidget *create_equipment_summary_card(equipment_t equipment)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_add_css_class(box, "equipment-summary-card");
  
  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_hexpand(left, TRUE);
  gtk_widget_set_halign(left, GTK_ALIGN_START);

  char id[ID_MAX + strlen("ID: ")];
  snprintf(id, ID_MAX + strlen("ID: "), "ID: EQ-%03d", equipment.id);
  
  GtkWidget *id_label = gtk_label_new(id);
  gtk_label_set_xalign(GTK_LABEL(id_label), 0.0);
  gtk_widget_add_css_class(id_label, "equipment-summary-id");

  GtkWidget *name_label = gtk_label_new(equipment.name);
  gtk_label_set_xalign(GTK_LABEL(name_label), 0.0);
  gtk_widget_add_css_class(name_label, "equipment-summary-name");

  gtk_box_append(GTK_BOX(left), id_label);
  gtk_box_append(GTK_BOX(left), name_label);

  GtkWidget *status = create_inventory_status_cell(equipment.status);
  gtk_widget_set_halign(status, GTK_ALIGN_END);
  gtk_widget_remove_css_class(status, "table-cell");

  gtk_box_append(GTK_BOX(header), left);
  gtk_box_append(GTK_BOX(header), status);
  
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 12);

  char type[STRING_MAX];
  snprintf(type, STRING_MAX, "%s", equipment_type_to_string(equipment.type));

  GtkWidget *type_box = create_summary_detail("Type:", type);
  gtk_widget_set_hexpand(type_box, TRUE);
  gtk_widget_set_halign(type_box, GTK_ALIGN_START);

  GtkWidget *ip_box = create_summary_detail("IP:", equipment.ip_address);

  GtkWidget *location_box = create_summary_detail("Location:", equipment.location);
  gtk_widget_set_hexpand(location_box, TRUE);
  gtk_widget_set_halign(location_box, GTK_ALIGN_START);
 
  GtkWidget *mac_box = create_summary_detail("MAC:", equipment.mac_address);

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
  
  ui_inventory_t *ui_inventory = (ui_inventory_t *) data;

  equipment_form_t *equipment_form = malloc(sizeof(equipment_form_t));
  if (equipment_form == NULL) return;

  equipment_form->application = ui_inventory->application;
  equipment_form->form = create_equipment_form(&equipment_form->application->equipments);
  dialog_config_t dialog_config = 
  {
      .window = ui_inventory->window,
      .form = equipment_form->form,
      .title = "Add Equipment",
      .dialog_action = 
      {
        .label = "Add Equipment",
        .icon = "assets/icon-add.svg",
        .css = "dialog-footer-add-button",
        .callback = G_CALLBACK(on_add_equipment_confirmed),
        .data = equipment_form
      }
  };

  equipment_form->mode = EQUIPMENT_FORM_ADD;
  equipment_form->selected_node = NULL;

  equipment_form->table = ui_inventory->table;
  equipment_form->dialog = create_dialog_window(dialog_config);

  g_object_set_data_full(G_OBJECT(equipment_form->dialog), "equipment-form", equipment_form, free); // ownership + free

  gtk_window_present(GTK_WINDOW(equipment_form->dialog));

  ui_inventory->selected_count = 0;
  ui_inventory->selected_node =  NULL;

  update_inventory_header(ui_inventory);
  refresh_inventory_table(equipment_form->table, &equipment_form->application->equipments);
}

static void on_edit_equipment_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter

  ui_inventory_t *ui_inventory = (ui_inventory_t *) data;

  if (ui_inventory->selected_node == NULL) return;

  equipment_form_t *equipment_form = malloc(sizeof(equipment_form_t));
  if (equipment_form == NULL) return;

  equipment_form->application = ui_inventory->application;
  equipment_form->form = create_equipment_form(&equipment_form->application->equipments);
  dialog_config_t dialog_config = 
  {
      .window = ui_inventory->window,
      .form = equipment_form->form,
      .title = "Edit Equipment",
      .dialog_action = 
      {
        .label = "Edit",
        .icon = "assets/icon-edit.svg",
        .css = "edit-button",
        .callback = G_CALLBACK(on_edit_equipment_confirmed),
        .data = equipment_form
      }
  };

  update_equipment_form(equipment_form->form, ui_inventory->selected_node->data);
  
  equipment_form->mode = EQUIPMENT_FORM_EDIT;
  equipment_form->selected_node = ui_inventory->selected_node;

  equipment_form->table = ui_inventory->table;
  equipment_form->dialog = create_dialog_window(dialog_config);

  g_object_set_data_full(G_OBJECT(equipment_form->dialog), "equipment-form", equipment_form, free); // ownership + free

  gtk_window_present(GTK_WINDOW(equipment_form->dialog));

  ui_inventory->selected_count = 0;
  ui_inventory->selected_node =  NULL;

  update_inventory_header(ui_inventory);
  refresh_inventory_table(equipment_form->table, &equipment_form->application->equipments);
}

static void on_remove_equipment_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter

  ui_inventory_t *ui_inventory = (ui_inventory_t *) data;

  if (ui_inventory->selected_node == NULL) return;

  equipment_form_t *equipment_form = malloc(sizeof(equipment_form_t));
  if (equipment_form == NULL) return;

  equipment_form->application = ui_inventory->application;
  equipment_form->form = create_remove_form(ui_inventory->selected_node->data);

  dialog_config_t dialog_config = 
  {
      .window = ui_inventory->window,
      .form = equipment_form->form,
      .title = "Remove Equipment",
      .dialog_action = 
      {
        .label = "Remove",
        .icon = "assets/icon-remove.svg",
        .css = "remove-button",
        .callback = G_CALLBACK(on_remove_equipment_confirmed),
        .data = equipment_form
      }
  };

  equipment_form->mode = EQUIPMENT_FORM_REMOVE;
  equipment_form->selected_node = ui_inventory->selected_node;

  equipment_form->table = ui_inventory->table;
  equipment_form->dialog = create_dialog_window(dialog_config);

  g_object_set_data_full(G_OBJECT(equipment_form->dialog), "equipment-form", equipment_form, free); // ownership + free

  gtk_window_present(GTK_WINDOW(equipment_form->dialog));

  ui_inventory->selected_count = 0;
  ui_inventory->selected_node =  NULL;

  update_inventory_header(ui_inventory);
  refresh_inventory_table(equipment_form->table, &equipment_form->application->equipments);
}

static void on_add_equipment_confirmed(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  equipment_form_t *equipment_form = (equipment_form_t *) data;

  equipment_t new;

  form_field_t fields[] = {
    { "entry-name", new.name },
    { "entry-vendor", new.vendor },
    { "entry-model", new.model },
    { "entry-ip", new.ip_address },
    { "entry-mac", new.mac_address },
    { "entry-location", new.location },
  };

  for (int i = 0; i < 6; i++)
  {
    GtkWidget *entry = g_object_get_data(G_OBJECT(equipment_form->form), fields[i].key);
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    snprintf(fields[i].dest, STRING_MAX, "%s", text);
  }

  if (validate_ip_address(new.ip_address) == FALSE) 
  {
    gtk_widget_add_css_class(g_object_get_data(G_OBJECT(equipment_form->form), "entry-ip"), "form-entry-error");
    return;
  }
  
  else 
  {
    gtk_widget_remove_css_class(g_object_get_data(G_OBJECT(equipment_form->form), "entry-ip"), "form-entry-error");
  }

  if (validate_mac_address(new.mac_address) == FALSE)
  {
    gtk_widget_add_css_class(g_object_get_data(G_OBJECT(equipment_form->form), "entry-mac"), "form-entry-error");
    return;
  }

  else 
  {
    gtk_widget_remove_css_class(g_object_get_data(G_OBJECT(equipment_form->form), "entry-mac"), "form-entry-error");
  }

  GtkWidget *dropdown_type = g_object_get_data(G_OBJECT(equipment_form->form), "dropdown-type");
  new.type = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_type));

  GtkWidget *dropdown_status = g_object_get_data(G_OBJECT(equipment_form->form), "dropdown-status");
  new.status = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_status));

  equipment_node_t *node = equipment_list_insert(&equipment_form->application->equipments, new);

  char id[ID_MAX];
  snprintf(id, ID_MAX, "EQ-%03d", node->data.id);

  hashmap_insert(&equipment_form->application->id_index, id, node);
  hashmap_insert(&equipment_form->application->ip_index, node->data.ip_address, node);
  hashmap_insert(&equipment_form->application->mac_index, node->data.mac_address, node);

  refresh_inventory_table(equipment_form->table, &equipment_form->application->equipments);

  gtk_window_destroy(GTK_WINDOW(equipment_form->dialog));
}

static void on_edit_equipment_confirmed(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  equipment_form_t *equipment_form = (equipment_form_t *) data;

  equipment_t update_equipment;

  form_field_t fields[] = {
    { "entry-name", update_equipment.name },
    { "entry-vendor", update_equipment.vendor },
    { "entry-model", update_equipment.model },
    { "entry-ip", update_equipment.ip_address },
    { "entry-mac", update_equipment.mac_address },
    { "entry-location", update_equipment.location },
  };

  for (int i = 0; i < 6; i++)
  {
    GtkWidget *entry = g_object_get_data(G_OBJECT(equipment_form->form), fields[i].key);
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    snprintf(fields[i].dest, STRING_MAX, "%s", text);
  }

  if (validate_ip_address(update_equipment.ip_address) == FALSE) 
  {
    gtk_widget_add_css_class(g_object_get_data(G_OBJECT(equipment_form->form), "entry-ip"), "form-entry-error");
    return;
  }
  
  else 
  {
    gtk_widget_remove_css_class(g_object_get_data(G_OBJECT(equipment_form->form), "entry-ip"), "form-entry-error");
  }

  if (validate_mac_address(update_equipment.mac_address) == FALSE)
  {
    gtk_widget_add_css_class(g_object_get_data(G_OBJECT(equipment_form->form), "entry-mac"), "form-entry-error");
    return;
  }

  else 
  {
    gtk_widget_remove_css_class(g_object_get_data(G_OBJECT(equipment_form->form), "entry-mac"), "form-entry-error");
  }

  GtkWidget *dropdown_type = g_object_get_data(G_OBJECT(equipment_form->form), "dropdown-type");
  update_equipment.type = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_type));

  GtkWidget *dropdown_status = g_object_get_data(G_OBJECT(equipment_form->form), "dropdown-status");
  update_equipment.status = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_status));

  equipment_node_t *node = equipment_form->selected_node;

  hashmap_update(&equipment_form->application->ip_index, node->data.ip_address, update_equipment.ip_address, node);
  hashmap_update(&equipment_form->application->mac_index, node->data.mac_address, update_equipment.mac_address, node);
  
  equipment_update(&node->data, update_equipment);

  refresh_inventory_table(equipment_form->table, &equipment_form->application->equipments);

  gtk_window_destroy(GTK_WINDOW(equipment_form->dialog));
}

static void on_remove_equipment_confirmed(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter

  equipment_form_t *equipment_form = (equipment_form_t *) data;

  equipment_node_t *node = equipment_form->selected_node;

  char id[ID_MAX];
  snprintf(id, ID_MAX, "EQ-%03d", node->data.id);

  hashmap_remove(&equipment_form->application->id_index, id);
  hashmap_remove(&equipment_form->application->ip_index, node->data.ip_address);
  hashmap_remove(&equipment_form->application->mac_index, node->data.mac_address);

  equipment_list_remove(&equipment_form->application->equipments, node);

  refresh_inventory_table(equipment_form->table, &equipment_form->application->equipments);

  gtk_window_destroy(GTK_WINDOW(equipment_form->dialog));
}

static void on_equipment_search_changed(GtkSearchEntry *search, gpointer data)
{
  ui_inventory_t *ui_inventory = (ui_inventory_t *) data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(search));

  equipment_node_t *node = NULL;

  switch (detect_search_type(text)) 
  {
    case SEARCH_ID:
      node = (equipment_node_t *) hashmap_get(&ui_inventory->application->id_index, text);
      break;
    case SEARCH_IP:
      node = (equipment_node_t *) hashmap_get(&ui_inventory->application->ip_index, text);
      break;
    case SEARCH_MAC:
      node = (equipment_node_t *) hashmap_get(&ui_inventory->application->mac_index, text);
      break;
    case SEARCH_INVALID:
      refresh_inventory_table(ui_inventory->table, &ui_inventory->application->equipments);
      return;
  }

  if (node == NULL) 
  {
    refresh_inventory_table(ui_inventory->table, &ui_inventory->application->equipments);
    return;
  }

  equipment_list_t list; // Temporary list
  equipment_list_init(&list);

  equipment_node_t *new = equipment_list_insert(&list, node->data);
  new->data = node->data;

  refresh_inventory_table(ui_inventory->table, &list);

  equipment_list_destroy(&list);
}

static void on_inventory_filter_changed(GObject *self, GParamSpec *pspec, gpointer data)
{
  (void)self; // unused parameter
  (void)pspec; // unused parameter

  ui_inventory_t *ui_inventory = (ui_inventory_t *) data;

  apply_inventory_filters(ui_inventory);
}

static void on_equipment_check_button_toggled(GtkCheckButton *button, gpointer data)
{
  ui_inventory_t *ui_inventory = (ui_inventory_t *) data;

  equipment_node_t *node = g_object_get_data(G_OBJECT(button), DATA_EQUIPMENT_NODE);

  bool is_active = gtk_check_button_get_active(button);

  // TODO: selected multiple rows 
  if (is_active == TRUE)
  {
    ui_inventory->selected_node = node;
    ui_inventory->selected_count = 1;
  }

  else 
  {
    ui_inventory->selected_node = NULL;
    ui_inventory->selected_count = 0;
  }

  update_inventory_header(ui_inventory);
}

