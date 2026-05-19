#include "ui_main_window.h"

#include "application.h"
#include "ui_widgets.h"

void create_main_window(GtkApplication *gui, gpointer data)
{
  application_t *application = (application_t *) data;

  GtkWidget *window = gtk_application_window_new(gui);
  gtk_window_set_title(GTK_WINDOW(window), "NetPulse NOC");
  gtk_window_set_default_size(GTK_WINDOW(window), 1440, 900);
  gtk_window_present(GTK_WINDOW(window));

  GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(window), main_box);

  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(header, -1, 52);
  gtk_box_append(GTK_BOX(main_box), header);

  GtkWidget *menu_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(menu_bar, -1, 44);
  gtk_box_append(GTK_BOX(main_box), menu_bar);
  
  // TODO: css style
  GtkWidget *dashboard_button = widget_create_icon_button("Dashboard", NULL, NULL); 
  GtkWidget *inventory_button = widget_create_icon_button("Inventory", NULL, NULL);
  GtkWidget *connectivity_button = widget_create_icon_button("Connectivity", NULL, NULL);
  GtkWidget *sensors_button = widget_create_icon_button("Sensors", NULL, NULL);
  GtkWidget *incidents_button = widget_create_icon_button("Incidents", NULL, NULL);
  GtkWidget *configurations_button = widget_create_icon_button("Configurations", NULL, NULL);
  // TODO: Technicians button 

  gtk_box_append(GTK_BOX(menu_bar), dashboard_button);
  gtk_box_append(GTK_BOX(menu_bar), inventory_button);
  gtk_box_append(GTK_BOX(menu_bar), connectivity_button);
  gtk_box_append(GTK_BOX(menu_bar), sensors_button);
  gtk_box_append(GTK_BOX(menu_bar), incidents_button);
  gtk_box_append(GTK_BOX(menu_bar), configurations_button);
}
