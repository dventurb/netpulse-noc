#include "ui_inventory.h"

#include "utils.h"
#include "ui_widgets.h"

static GtkWidget *create_side_bar(application_t *application);
static GtkWidget *create_content(application_t *application);
static GtkWidget *create_invetory_header(application_t *application);
static GtkWidget *create_inventory_table(application_t *application);

GtkWidget *create_page_inventory(application_t *application)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  GtkWidget *side_bar = create_side_bar(application);
  GtkWidget *content = create_content(application);

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

static GtkWidget *create_content(application_t *application)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 1160, -1);

  GtkWidget *header = create_invetory_header(application);
  GtkWidget *table = create_inventory_table(application);

  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), table);

  return box;
}

static GtkWidget *create_invetory_header(application_t *application)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_margin_top(box, 24);

  GtkWidget *title = gtk_label_new("Equipment Inventory");
  gtk_widget_add_css_class(title, "inventory-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  GtkWidget *add_device_button = widget_create_secondary_button("Add device", "assets/icon-add-device.svg", "secondary-button");
  gtk_widget_set_margin_end(add_device_button, 24);
  
  gtk_box_append(GTK_BOX(box), title);
  gtk_box_append(GTK_BOX(box), add_device_button);

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
