#include "ui_inventory.h"

#include "utils.h"

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

  GtkWidget *title = gtk_label_new("Equipment Inventory");
  gtk_widget_add_css_class(title, "inventory-title");
  
  gtk_box_append(GTK_BOX(box), title);

  return box;
}

// TODO: Not finished yet.
static GtkWidget *create_inventory_table(application_t *application)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

  const char *headers[] = {
    "ID",
    "NAME",
    "TYPE",
    "IP ADDRESS",
    "MAC ADDRESS",
    "LOCATION",
    "LAST CHECK"
  };

  for (int i = 0; i < 7; i++) {
    GtkWidget *label = gtk_label_new(headers[i]);
    gtk_widget_add_css_class(label, "header-label");
    gtk_grid_attach(GTK_GRID(grid), label, i, 0, 1, 1);
  }

  if (application->equipments.head == NULL) return grid;

  equipment_t equipment = application->equipments.head->data;

  for (int i = 1; i <= application->equipments.count; i++) {
    char id[10];
    snprintf(id, sizeof(id), "%d", equipment.id);

    char datetime[DATETIME_MAX];
    get_datetime(equipment.last_check, datetime);

    GtkWidget *labels[] = {
      gtk_label_new(id),
      gtk_label_new(equipment.name),
      gtk_label_new(equipment.type),
      gtk_label_new(equipment.ip_address),
      gtk_label_new(equipment.mac_address),
      gtk_label_new(equipment.location),
      gtk_label_new(datetime)
    };

    for (int j = 0; j < 7; j++) {
      gtk_grid_attach(GTK_GRID(grid), labels[j], j, i, 1, 1);
    }
  }

  return grid;
}
