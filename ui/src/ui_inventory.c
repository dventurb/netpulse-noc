#include "ui_inventory.h"

#include "utils.h"
#include "ui_widgets.h"
#include "macros.h"

static GtkWidget *create_side_bar(application_t *application);
static GtkWidget *create_content(ui_t *ui);
static GtkWidget *create_inventory_header(ui_t *ui);
static GtkWidget *create_inventory_table(application_t *application);
static GtkWidget *create_equipment_form(equipment_list_t *equipments);
static void refresh_inventory_table(GtkWidget *grid, equipment_list_t *equipments);

// Callbacks
static void on_add_equipment_button_clicked(GtkButton *button, gpointer data);
static void on_submit_equipment_button_clicked(GtkButton *button, gpointer data);

// TODO: look for better way
static const char* const status[] = { "Failed", "Maintenance", "Operational", "Disabled", NULL };
static const char* const types[] = { "Select type...", "Router", "Firewall", "Switch", "Access Point", "Server", "NAS", "UPS", "IP Camera", "Printer", "Other", NULL };
static const char* const headers[] = { "ID", "NAME", "TYPE", "VENDOR", "MODEL", "IP ADDRESS", "MAC ADDRESS", "LOCATION", "STATUS", "LAST CHECK" };


GtkWidget *create_page_inventory(ui_t *ui)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  GtkWidget *side_bar = create_side_bar(ui->application);
  GtkWidget *content = create_content(ui);

  gtk_box_append(GTK_BOX(box), side_bar);
  gtk_box_append(GTK_BOX(box), content);

  return box;
}

static GtkWidget *create_side_bar(application_t *application)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 280, -1);

  return box;
}

static GtkWidget *create_content(ui_t *ui)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 1160, -1);

  GtkWidget *header = create_inventory_header(ui);
  GtkWidget *table = create_inventory_table(ui->application);

  g_object_set_data(G_OBJECT(ui->stack), "inventory-table", table);

  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), table);

  return box;
}

static GtkWidget *create_inventory_header(ui_t *ui)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);

  GtkWidget *title = gtk_label_new("Equipment Inventory");
  gtk_widget_add_css_class(title, "inventory-title");
  gtk_widget_set_margin_start(title, 24);
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  GtkWidget *add_equipment_button = create_secondary_button("Add Equipment", "assets/icon-add-device.svg", "secondary-button");
  gtk_widget_set_margin_end(add_equipment_button, 24);
  g_signal_connect(add_equipment_button, "clicked", G_CALLBACK(on_add_equipment_button_clicked), ui);
  
  gtk_box_append(GTK_BOX(box), title);
  gtk_box_append(GTK_BOX(box), add_equipment_button);

  return box;
}

// TODO: Not finished yet.
static GtkWidget *create_inventory_table(application_t *application)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_widget_set_margin_end(grid, 24);
  gtk_widget_add_css_class(grid, "table");

  GtkWidget *select_all_button = gtk_check_button_new();
  gtk_widget_add_css_class(select_all_button, "table-header-checkbox");
  gtk_grid_attach(GTK_GRID(grid), select_all_button, 0, 0, 1, 1);

  for (int i = 0; i < 10; i++) {
    GtkWidget *label = gtk_label_new(headers[i]);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_add_css_class(label, "table-header-cell");
    gtk_grid_attach(GTK_GRID(grid), label, i + 1, 0, 1, 1);
  }

  equipment_node_t *node = application->equipments.head;
  if (node == NULL) return grid;

  for (int i = 1; i <= application->equipments.count; i++) {
    equipment_t equipment = node->data;

    GtkWidget *check_button = gtk_check_button_new();
    g_object_set_data(G_OBJECT(check_button), "equipment", (void *)node);

    char id[ID_MAX];
    snprintf(id, ID_MAX, "EQ-%03d", equipment.id);

    char datetime[DATETIME_MAX];
    get_datetime(equipment.last_check, datetime);

    GtkWidget *columns[] = {
      check_button,
      gtk_label_new(id),
      gtk_label_new(equipment.name),
      gtk_label_new(equipment_type_to_string(equipment.type)),
      gtk_label_new(equipment.vendor),
      gtk_label_new(equipment.model),
      gtk_label_new(equipment.ip_address),
      gtk_label_new(equipment.mac_address),
      gtk_label_new(equipment.location),
      gtk_label_new(equipment_status_to_string(equipment.status)),
      gtk_label_new(datetime)
    };

    for (int j = 0; j < 11; j++) {
      gtk_grid_attach(GTK_GRID(grid), columns[j], j, i, 1, 1);
    }

    node = node->next;
  }

  return grid;
}

static GtkWidget *create_equipment_form(equipment_list_t *equipments)
{
  GtkWidget *grid = gtk_grid_new();
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

  char id[ID_MAX];
  snprintf(id, ID_MAX, "EQ-%03d", equipments->next_id);
  gtk_editable_set_text(GTK_EDITABLE(entry_id), id);

  GtkWidget *entry_name = create_text_field(grid, "Equipment Name", "Core-Switch-01", 0, 1);
  g_object_set_data(G_OBJECT(grid), "entry-name", entry_name);

  GtkWidget *dropdown_type = create_dropdown_field(grid, "Type", types, 1, 0);
  g_object_set_data(G_OBJECT(grid), "dropdown-type", dropdown_type);

  GtkWidget *entry_vendor = create_text_field(grid, "Vendor", "Cisco", 1, 1);
  g_object_set_data(G_OBJECT(grid), "entry-vendor", entry_vendor);

  GtkWidget *entry_model = create_text_field(grid, "Model", "Catalyst 9300", 2, 0);
  g_object_set_data(G_OBJECT(grid), "entry-model", entry_model);

  GtkWidget *entry_ip = create_text_field(grid, "IP Address", "192.168.1.1", 2, 1);
  g_object_set_data(G_OBJECT(grid), "entry-ip", entry_ip);

  GtkWidget *entry_mac = create_text_field(grid, "MAC Address", "00:1A:2B:3C:4D:5E", 3, 0); 
  g_object_set_data(G_OBJECT(grid), "entry-mac", entry_mac);

  GtkWidget *entry_location = create_text_field(grid, "Location", "Data Center Rack A4", 3, 1);
  g_object_set_data(G_OBJECT(grid), "entry-location", entry_location);

  GtkWidget *dropdown_status = create_dropdown_field(grid, "Status", status, 4, 0);
  g_object_set_data(G_OBJECT(grid), "dropdown-status", dropdown_status);

  return grid;
}

static void refresh_inventory_table(GtkWidget *grid, equipment_list_t *equipments)
{
  remove_all_items_table(grid);

  equipment_node_t *node = equipments->head;
  if (node == NULL) return;

  for (int i = 1; i <= equipments->count; i++) {
    equipment_t equipment = node->data;

    GtkWidget *check_button = gtk_check_button_new();
    g_object_set_data(G_OBJECT(check_button), "equipment", (void *)node);

    char id[ID_MAX];
    snprintf(id, ID_MAX, "EQ-%03d", equipment.id);

    char datetime[DATETIME_MAX];
    get_datetime(equipment.last_check, datetime);

    GtkWidget *columns[] = {
      check_button,
      gtk_label_new(id),
      gtk_label_new(equipment.name),
      gtk_label_new(equipment_type_to_string(equipment.type)),
      gtk_label_new(equipment.vendor),
      gtk_label_new(equipment.model),
      gtk_label_new(equipment.ip_address),
      gtk_label_new(equipment.mac_address),
      gtk_label_new(equipment.location),
      gtk_label_new(equipment_status_to_string(equipment.status)),
      gtk_label_new(datetime)
    };

    for (int j = 0; j < 11; j++) {
      if (j != 0) gtk_widget_add_css_class(columns[j], "table-cell");
      else gtk_widget_add_css_class(columns[0], "table-checkbox");

      gtk_grid_attach(GTK_GRID(grid), columns[j], j, i, 1, 1);
    }

    node = node->next;
  }
}

static void on_add_equipment_button_clicked(GtkButton *button, gpointer data)
{
  ui_t *ui = (ui_t *) data;

  equipment_form_t *equipment_form = malloc(sizeof(equipment_form_t));
  if (equipment_form == NULL) return;

  equipment_form->equipments = &ui->application->equipments;
  equipment_form->form = create_equipment_form(equipment_form->equipments);

  GtkWidget *dialog = create_dialog_window(ui->window, equipment_form->form, "Add Equipment", G_CALLBACK(on_submit_equipment_button_clicked), equipment_form);

  equipment_form->dialog = dialog;

  GtkWidget *table = g_object_get_data(G_OBJECT(ui->stack), "inventory-table");
  g_object_set_data(G_OBJECT(equipment_form->dialog), "inventory-table", table);

  g_object_set_data_full(G_OBJECT(dialog), "equipment-form", equipment_form, free); // ownership + free

  gtk_window_present(GTK_WINDOW(dialog));
}

static void on_submit_equipment_button_clicked(GtkButton *button, gpointer data)
{
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

  // TODO: Check if position != 0
  GtkWidget *dropdown_type = g_object_get_data(G_OBJECT(equipment_form->form), "dropdown-type");
  new.type = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_type)) - 1; // The placeholder don't count

  GtkWidget *dropdown_status = g_object_get_data(G_OBJECT(equipment_form->form), "dropdown-status");
  new.status = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_status));

  equipment_list_insert(equipment_form->equipments, new);

  GtkWidget *table = g_object_get_data(G_OBJECT(equipment_form->dialog), "inventory-table");

  refresh_inventory_table(table, equipment_form->equipments);

  gtk_window_destroy(GTK_WINDOW(equipment_form->dialog));
}
