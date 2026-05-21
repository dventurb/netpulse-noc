#include "ui_inventory.h"

#include "utils.h"
#include "ui_widgets.h"

static GtkWidget *create_side_bar(application_t *application);
static GtkWidget *create_content(ui_t *ui);
static GtkWidget *create_inventory_header(ui_t *ui);
static GtkWidget *create_inventory_table(application_t *application);
static GtkWidget *create_dialog_window(GtkWidget *window);

// Callbacks
static void on_add_equipment_button_clicked(GtkButton *button, gpointer data);

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
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  GtkWidget *add_equipment_button = widget_create_secondary_button("Add equipment", "assets/icon-add-device.svg", "secondary-button");
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
  gtk_widget_add_css_class(grid, "table");

  GtkWidget *select_all_button = gtk_check_button_new();
  gtk_widget_add_css_class(select_all_button, "table-checkbox");
  gtk_grid_attach(GTK_GRID(grid), select_all_button, 0, 0, 1, 1);

  const char *headers[] = {
    "ID",
    "NAME",
    "TYPE",
    "VENDOR",
    "MODEL",
    "IP ADDRESS",
    "MAC ADDRESS",
    "LOCATION",
    "LAST CHECK"
  };

  for (int i = 0; i < 9; i++) {
    GtkWidget *label = gtk_label_new(headers[i]);
    gtk_widget_add_css_class(label, "table-header");
    gtk_grid_attach(GTK_GRID(grid), label, i + 1, 0, 1, 1);
  }

  equipment_node_t *node = application->equipments.head;
  if (node == NULL) return grid;

  for (int i = 1; i <= application->equipments.count; i++) {
    equipment_t equipment = node->data;

    GtkWidget *check_button = gtk_check_button_new();
    g_object_set_data(G_OBJECT(check_button), "equipment", (void *)node);

    char id[10];
    snprintf(id, sizeof(id), "%d", equipment.id);

    char datetime[DATETIME_MAX];
    get_datetime(equipment.last_check, datetime);

    GtkWidget *columns[] = {
      check_button,
      gtk_label_new(id),
      gtk_label_new(equipment.name),
      gtk_label_new(equipment.type),
      gtk_label_new(equipment.vendor),
      gtk_label_new(equipment.model),
      gtk_label_new(equipment.ip_address),
      gtk_label_new(equipment.mac_address),
      gtk_label_new(equipment.location),
      gtk_label_new(datetime)
    };

    for (int j = 0; j < 7; j++) {
      gtk_grid_attach(GTK_GRID(grid), columns[j], j, i, 1, 1);
    }

    node = node->next;
  }

  return grid;
}

static void on_add_equipment_button_clicked(GtkButton *button, gpointer data)
{
  ui_t *ui = (ui_t *) data;

  GtkWidget *dialog = create_dialog_window(ui->window);

  gtk_window_present(GTK_WINDOW(dialog));
}

static GtkWidget *create_dialog_window(GtkWidget *window)
{
  GtkWidget *dialog = gtk_window_new();
  gtk_widget_add_css_class(dialog, "dialog");
  gtk_window_set_default_size(GTK_WINDOW(dialog), 672, 640);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
  gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);
  
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(dialog), box);

  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(header, "dialog-header");

  GtkWidget *title = gtk_label_new("Add Equipment");
  gtk_widget_add_css_class(title, "dialog-header-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);
  gtk_widget_set_margin_start(title, 24);

  GtkWidget *close_button = gtk_button_new_with_label("X");
  gtk_widget_add_css_class(close_button, "dialog-header-button");

  gtk_box_append(GTK_BOX(header), title);
  gtk_box_append(GTK_BOX(header), close_button);

  gtk_box_append(GTK_BOX(box), header);

  GtkWidget *form = gtk_grid_new();
  gtk_widget_set_margin_start(form, 24);
  gtk_widget_set_margin_end(form, 24);
  gtk_widget_set_margin_top(form, 24);
  gtk_widget_set_margin_bottom(form, 40);
  gtk_grid_set_column_spacing(form, 24);
  gtk_grid_set_row_spacing(form, 24);
  gtk_widget_add_css_class(form, "dialog-form");

  GtkWidget *entry_id = widget_add_text_field(form, "Equipment ID", NULL, 0, 0);
  GtkWidget *entry_name = widget_add_text_field(form, "Equipment Name", "Core-Switch-01", 0, 1);

  const char *types[] = {
    "Select type...",
    "Router",
    "Switch",
    "Acess Point",
    "Firewall",
    "Server",
    "NAS",
    "Printer",
    "IP Camera",
    "UPS",
    "Other",
    NULL
  };

  GtkWidget *dropdown_type = widget_add_dropdown_field(form, "Type", types, 1, 0);

  GtkWidget *entry_vendor = widget_add_text_field(form, "Vendor", "Cisco", 1, 1);

  GtkWidget *entry_model = widget_add_text_field(form, "Model", "Catalyst 9300", 2, 0);
  GtkWidget *entry_ip = widget_add_text_field(form, "IP Address", "192.168.1.1", 2, 1);

  GtkWidget *entry_mac = widget_add_text_field(form, "Mac Address", "00:1A:2B:3C:4D:5E", 3, 0);
  GtkWidget *entry_location = widget_add_text_field(form, "Location", "Data Center Rack A4", 3, 1);

  const char *status[] = {
    "Operational",
    "Maintenance",
    "Failed",
    "Disabled",
    NULL
  };

  GtkWidget *dropdown_status = widget_add_dropdown_field(form, "Status", status, 4, 0);

  gtk_box_append(GTK_BOX(box), form);

  GtkWidget *footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_add_css_class(footer, "dialog-footer");

  GtkWidget *cancel_button = widget_create_secondary_button("Cancel", NULL, "footer-cancel-button");
  gtk_widget_set_hexpand(cancel_button, TRUE);
  gtk_widget_set_halign(cancel_button, GTK_ALIGN_END);

  GtkWidget *add_equipment_button = widget_create_secondary_button("Add Equipment", "assets/icon-add-device.svg", "footer-add-button");
  gtk_widget_set_margin_end(add_equipment_button, 24);

  gtk_box_append(GTK_BOX(footer), cancel_button);
  gtk_box_append(GTK_BOX(footer), add_equipment_button);

  gtk_box_append(GTK_BOX(box), footer);

  return dialog;
}
