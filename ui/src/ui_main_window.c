#include "ui_main_window.h"

#include "application.h"
#include "ui_widgets.h"

static GtkWidget *create_header(void);
static GtkWidget *create_menu_bar(void);

void create_main_window(GtkApplication *gui, gpointer data)
{
  application_t *application = (application_t *) data;

  GtkWidget *window = gtk_application_window_new(gui);
  gtk_window_set_title(GTK_WINDOW(window), "NetPulse NOC");
  gtk_window_set_default_size(GTK_WINDOW(window), 1440, 900);

  GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(window), main_box);

  GtkWidget *header = create_header();
  GtkWidget *menu_bar = create_menu_bar();
  
  gtk_box_append(GTK_BOX(main_box), header);
  gtk_box_append(GTK_BOX(main_box), menu_bar);

  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, "styles/style.css");
  gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), 800);

  g_object_unref(provider);

  gtk_window_present(GTK_WINDOW(window));
}

static GtkWidget *create_header(void)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(box, -1, 52);
  gtk_widget_add_css_class(box, "header");

  return box;
}

static GtkWidget *create_menu_bar(void)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(box, -1, 44);
  gtk_widget_add_css_class(box, "menu-bar");

  // TODO: css style
  GtkWidget *dashboard_button = widget_create_primary_button("Dashboard", "menu-button"); 
  GtkWidget *inventory_button = widget_create_primary_button("Inventory", "menu-button");
  GtkWidget *connectivity_button = widget_create_primary_button("Connectivity", "menu-button");
  GtkWidget *sensors_button = widget_create_primary_button("Sensors", "menu-button");
  GtkWidget *incidents_button = widget_create_primary_button("Incidents", "menu-button");
  GtkWidget *configurations_button = widget_create_primary_button("Configurations", "menu-button");
  // TODO: Technicians button 

  gtk_box_append(GTK_BOX(box), dashboard_button);
  gtk_box_append(GTK_BOX(box), inventory_button);
  gtk_box_append(GTK_BOX(box), connectivity_button);
  gtk_box_append(GTK_BOX(box), sensors_button);
  gtk_box_append(GTK_BOX(box), incidents_button);
  gtk_box_append(GTK_BOX(box), configurations_button);

  return box;
}
