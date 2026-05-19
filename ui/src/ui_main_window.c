#include "ui_main_window.h"

#include "ui_widgets.h"
#include "ui_inventory.h"

static GtkWidget *create_header(void);
static GtkWidget *create_menu_bar(application_t *application, GtkWidget *stack);
static GtkWidget *create_menu_button(menu_context_t *menu, const char *label);

// Callbacks
static void on_menu_button_clicked(GtkButton *button, gpointer data);

void create_main_window(GtkApplication *gui, gpointer data)
{
  application_t *application = (application_t *) data;

  GtkWidget *window = gtk_application_window_new(gui);
  gtk_window_set_title(GTK_WINDOW(window), "NetPulse NOC");
  gtk_window_set_default_size(GTK_WINDOW(window), 1440, 900);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(window), box);

  GtkWidget *stack = gtk_stack_new();
  gtk_widget_set_vexpand(stack, TRUE);

  GtkWidget *header = create_header();
  GtkWidget *menu_bar = create_menu_bar(application, stack);
  
  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), menu_bar);
  gtk_box_append(GTK_BOX(box), stack);

  gtk_stack_add_named(GTK_STACK(stack), create_page_inventory(application), "Inventory");
  gtk_stack_set_visible_child_name(GTK_STACK(stack), "Inventory");

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

  GtkWidget *logo = gtk_picture_new_for_filename("assets/logo.svg");

  gtk_widget_set_halign(logo, GTK_ALIGN_START);
  gtk_widget_set_valign(logo, GTK_ALIGN_CENTER);
  gtk_widget_set_margin_start(logo, 24);
  gtk_widget_set_margin_top(logo, 4);
  gtk_widget_set_margin_bottom(logo, 4);

  gtk_box_append(GTK_BOX(box), logo);

  return box;
}

static GtkWidget *create_menu_bar(application_t *application, GtkWidget *stack)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(box, -1, 44);
  gtk_widget_add_css_class(box, "menu-bar");

  // TODO: Technicians button 
  const char *modules[] = {
    "Dashboard",
    "Inventory",
    "Connectivity",
    "Sensors",
    "Incidents",
    "Configurations"
  };

  menu_context_t *menu = malloc(sizeof(menu_context_t));
  if (menu == NULL) return NULL;

  menu->application = application;
  menu->stack = stack;
  menu->count = 6;

  for (int i = 0; i < menu->count; i++)
  {
    GtkWidget *button = create_menu_button(menu, modules[i]); 
    menu->buttons[i] = button;
    gtk_box_append(GTK_BOX(box), button);
  }

  g_object_set_data_full(G_OBJECT(box), "menu-context", menu, free); // ownership + free

  return box;
}

static GtkWidget *create_menu_button(menu_context_t *menu, const char *label) 
{
  GtkWidget *button = widget_create_primary_button(label, "menu-button");
  g_signal_connect(button, "clicked", G_CALLBACK(on_menu_button_clicked), menu);

  g_object_set_data(G_OBJECT(button), "target-page", label);

  return button;
}

static void on_menu_button_clicked(GtkButton *button, gpointer data)
{
  // menu_context_t *menu = (menu_context_t *) data;
}
