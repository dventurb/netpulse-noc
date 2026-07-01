#include "main_window.h"

#include "app.h"
#include "style.h"

#include "toggle_button.h"
#include "profile_box.h"

static void main_window_init(main_window_t *main_win, app_t *app);

static void build_window(main_window_t *main_win);
static void build_layout(main_window_t *main_win);
static void setup_pages(main_window_t *main_win);

static GtkWidget *build_header(main_window_t *main_win);
static GtkWidget *build_menu_bar(main_window_t *main_win);
static GtkWidget *build_menu_button(main_window_t *main_win, const page_config_t *page);
static GtkWidget *build_technician_profile_box(main_window_t *main_win);

static void add_equipment_page(main_window_t *main_win);
static void add_connectivity_page(main_window_t *main_win);
static void add_sensor_page(main_window_t *main_win);
static void add_incident_page(main_window_t *main_win);
static void add_configuration_page(main_window_t *main_win);

static void refresh_equipment(main_window_t *main_win);
static void refresh_sensor(main_window_t *main_win);
static void refresh_incident(main_window_t *main_win);
static void refresh_configuration(main_window_t *main_win);

// Callbacks
static void on_menu_button_clicked(GtkButton *button, gpointer data);

static const page_config_t pages[PAGE_COUNT] = 
{
  { PAGE_DASHBOARD,     "Dashboard",      "DASHBOARD",     NULL },
  { PAGE_EQUIPMENT,     "Inventory",      "EQUIPMENT",     refresh_equipment },
  { PAGE_CONNECTIVITY,  "Connectivity",   "CONNECTIVITY",  NULL },
  { PAGE_SENSOR,        "Sensors",        "SENSOR",        refresh_sensor },
  { PAGE_INCIDENT,      "Incidents",      "INCIDENT",      refresh_incident },
  { PAGE_CONFIGURATION, "Configurations", "CONFIGURATION", refresh_configuration },
};


main_window_t *main_window_create(GtkApplication *gtk_app, app_t *app)
{
  main_window_t *main_win = malloc(sizeof(main_window_t));
  if (main_win == NULL) return NULL;

  main_window_init(main_win, app);

  gtk_application_add_window(gtk_app, main_win->window);

  g_object_set_data_full(G_OBJECT(main_win->window), "user-interface", main_win, free); // ownership + free

  return main_win;
}

static void main_window_init(main_window_t *main_win, app_t *app)
{
  main_win->app = app;

  build_window(main_win);
  build_layout(main_win);
  setup_pages(main_win);
  load_styles();

  gtk_stack_set_visible_child_name(main_win->stack, "EQUIPMENT");
}

static void build_window(main_window_t *main_win)
{
  main_win->window = GTK_WINDOW(gtk_window_new());
  gtk_window_set_title(main_win->window, "NetPulse NOC");
  gtk_window_set_default_size(main_win->window, 1440, 900);
}

static void build_layout(main_window_t *main_win)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_hexpand(container, TRUE);
  gtk_window_set_child(main_win->window, container);

  main_win->stack = GTK_STACK(gtk_stack_new());

  GtkWidget *header = build_header(main_win);
  GtkWidget *menu_bar = build_menu_bar(main_win);
  
  gtk_box_append(GTK_BOX(container), header);
  gtk_box_append(GTK_BOX(container), menu_bar);
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(main_win->stack));
}

static void setup_pages(main_window_t *main_win)
{
  add_equipment_page(main_win);
  add_incident_page(main_win);
  add_connectivity_page(main_win);
  add_sensor_page(main_win);
  add_configuration_page(main_win);
}

static GtkWidget *build_header(main_window_t *main_win)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(box, -1, 52);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_add_css_class(box, "header");

  GtkWidget *logo = gtk_picture_new_for_filename("assets/logo.svg");
  gtk_widget_set_halign(logo, GTK_ALIGN_START);
  gtk_widget_set_valign(logo, GTK_ALIGN_CENTER);
  gtk_widget_set_margin_start(logo, 24);
  gtk_widget_set_margin_top(logo, 4);
  gtk_widget_set_margin_bottom(logo, 4);

  GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_hexpand(spacer, TRUE);

  GtkWidget *technician_box = build_technician_profile_box(main_win);

  gtk_box_append(GTK_BOX(box), logo);
  gtk_box_append(GTK_BOX(box), spacer);
  gtk_box_append(GTK_BOX(box), technician_box);

  return box;
}

static GtkWidget *build_menu_bar(main_window_t *main_win)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(box, -1, 44);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_add_css_class(box, "menu-bar");

  for (int i = 0; i < PAGE_COUNT; i++)
  {
    main_win->nav_buttons[i] = GTK_TOGGLE_BUTTON(build_menu_button(main_win, &pages[i]));

    gtk_box_append(GTK_BOX(box), GTK_WIDGET(main_win->nav_buttons[i]));
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(main_win->nav_buttons[1]), TRUE); // Inventory Button to 'pressed in' 
  
  return box;
}

static GtkWidget *build_menu_button(main_window_t *main_win, const page_config_t *page) 
{
  GtkWidget *button = toggle_button_new(page->text, NULL, "menu-button");
  g_signal_connect(button, "clicked", G_CALLBACK(on_menu_button_clicked), main_win);

  g_object_set_data(G_OBJECT(button), "page-config", (void *)page);

  return button;
}

static GtkWidget *build_technician_profile_box(main_window_t *main_win)
{
  technician_t *current_user = main_win->app->data.current_user;

  return profile_box_new(current_user->name, current_user->avatar_path);
}

static void add_equipment_page(main_window_t *main_win)
{
  equipment_controller_init(&main_win->equipment_ctrl, &main_win->equipment_view, &main_win->app->data);

  GtkBox *container = equipment_view_create(&main_win->equipment_view, &main_win->equipment_ctrl);

  gtk_stack_add_named(main_win->stack, GTK_WIDGET(container), "EQUIPMENT");
}

static void add_connectivity_page(main_window_t *main_win)
{
  connectivity_controller_init(&main_win->connectivity_ctrl, &main_win->connectivity_view, &main_win->app->data);

  GtkBox *container = connectivity_view_create(&main_win->connectivity_view, &main_win->connectivity_ctrl);

  gtk_stack_add_named(main_win->stack, GTK_WIDGET(container), "CONNECTIVITY");
}

static void add_sensor_page(main_window_t *main_win)
{
 sensor_controller_init(&main_win->sensor_ctrl, &main_win->sensor_view, &main_win->app->data);

  GtkBox *container = sensor_view_create(&main_win->sensor_view, &main_win->sensor_ctrl);

  gtk_stack_add_named(main_win->stack, GTK_WIDGET(container), "SENSOR");
}

static void add_incident_page(main_window_t *main_win)
{
  incident_controller_init(&main_win->incident_ctrl, &main_win->incident_view, &main_win->app->data);

  GtkBox *container = incident_view_create(&main_win->incident_view, &main_win->incident_ctrl);

  gtk_stack_add_named(main_win->stack, GTK_WIDGET(container), "INCIDENT");
}

static void add_configuration_page(main_window_t *main_win)
{
  configuration_controller_init(&main_win->config_ctrl, &main_win->config_view, &main_win->app->data);

  GtkBox *container = configuration_view_create(&main_win->config_view, &main_win->config_ctrl);

  gtk_stack_add_named(main_win->stack, GTK_WIDGET(container), "CONFIGURATION");
}

static void refresh_equipment(main_window_t *main_win)
{
  equipment_view_refresh(&main_win->equipment_view);
}

static void refresh_sensor(main_window_t *main_win)
{
  sensor_view_refresh(&main_win->sensor_view);
}

static void refresh_incident(main_window_t *main_win)
{
  incident_view_refresh(&main_win->incident_view);
}

static void refresh_configuration(main_window_t *main_win)
{
  configuration_view_refresh(&main_win->config_view);
}

static void on_menu_button_clicked(GtkButton *button, gpointer data)
{
  main_window_t *main_win = (main_window_t *) data;

  for (int i = 0; i < PAGE_COUNT; i++) 
  {
    if (GTK_WIDGET(main_win->nav_buttons[i]) != GTK_WIDGET(button))
      gtk_toggle_button_set_active(main_win->nav_buttons[i], FALSE);
    else 
       gtk_toggle_button_set_active(main_win->nav_buttons[i], TRUE);
  }

  page_config_t *page = g_object_get_data(G_OBJECT(button), "page-config");

  gtk_stack_set_visible_child_name(main_win->stack, page->stack_name);
  
  if (page->refresh) page->refresh(main_win);
}
