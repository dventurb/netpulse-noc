#include "main_window.h"

#include "ui_widgets.h"

static void main_window_init(main_window_t *main_window, gpointer data);

static GtkWidget *build_header(void);
static GtkWidget *build_menu_bar(main_window_t *main_window);
static GtkWidget *build_menu_button(main_window_t *main_window, const char *text, const char *page_name);

// Callbacks
static void on_menu_button_clicked(GtkButton *button, gpointer data);


void main_window_create(GtkApplication *gui, gpointer data)
{
  main_window_t *main_window = malloc(sizeof(main_window_t));
  if (main_window == NULL) return;

  main_window_init(main_window, data);

  gtk_application_add_window(gui, main_window->window);

  g_object_set_data_full(G_OBJECT(main_window->window), "user-interface", main_window, free); // ownership + free

  gtk_window_present(main_window->window);
}

static void main_window_init(main_window_t *main_window, gpointer data)
{
  main_window->window = GTK_WINDOW(gtk_window_new());
  gtk_window_set_title(main_window->window, "NetPulse NOC");
  gtk_window_set_default_size(main_window->window, 1440, 900);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(main_window->window, box);

  main_window->stack = GTK_STACK(gtk_stack_new());

  GtkWidget *header = build_header();
  GtkWidget *menu_bar = build_menu_bar(main_window);
  
  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), menu_bar);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(main_window->stack));


  // EQUIPMENT - Controller + Model
  equipment_controller_init(&main_window->equipment_ctrl, &main_window->equipment_view, (void *)data);
  GtkBox *equip_container = equipment_view_create(&main_window->equipment_view, &main_window->equipment_ctrl);
  gtk_stack_add_named(main_window->stack, GTK_WIDGET(equip_container), "EQUIPMENT");


  // INCIDENT - Controller + Model
  incident_controller_init(&main_window->incident_ctrl, &main_window->incident_view, (void *)data);
  GtkBox *inc_container = incident_view_create(&main_window->incident_view, &main_window->incident_ctrl);
  gtk_stack_add_named(main_window->stack, GTK_WIDGET(inc_container), "INCIDENT");


  // CONNECTIVITY - Controller + Model
  connectivity_controller_init(&main_window->connectivity_ctrl, &main_window->connectivity_view, (void *)data);
  GtkBox *conn_container = connectivity_view_create(&main_window->connectivity_view, &main_window->connectivity_ctrl);
  gtk_stack_add_named(main_window->stack, GTK_WIDGET(conn_container), "CONNECTIVITY");


  // SENSOR - Controller + Model
  sensor_controller_init(&main_window->sensor_ctrl, &main_window->sensor_view, (void *)data);
  GtkBox *sensor_container = sensor_view_create(&main_window->sensor_view, &main_window->sensor_ctrl);
  gtk_stack_add_named(main_window->stack, GTK_WIDGET(sensor_container), "SENSOR");


  gtk_stack_set_visible_child_name(main_window->stack, "EQUIPMENT");

  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, "styles/style.css");
  gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), 800);

  g_object_unref(provider);
}

static GtkWidget *build_header(void)
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

static GtkWidget *build_menu_bar(main_window_t *main_window)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(box, -1, 44);
  gtk_widget_add_css_class(box, "menu-bar");

  // TODO: Technicians button 
  const char *modules[PAGE_COUNT] = {
    "Dashboard",
    "Inventory",
    "Connectivity",
    "Sensors",
    "Incidents",
    "Configurations"
  };

  const char *page_name[PAGE_COUNT] = {
    "DASHBOARD",
    "EQUIPMENT",
    "CONNECTIVITY",
    "SENSOR",
    "INCIDENT",
    "CONFIGURATION"
  };

  for (int i = 0; i < PAGE_COUNT; i++)
  {
    main_window->nav_buttons[i] = GTK_TOGGLE_BUTTON(build_menu_button(main_window, modules[i], page_name[i]));
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(main_window->nav_buttons[i]));
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(main_window->nav_buttons[1]), TRUE); // Inventory Button to 'pressed in' 

  return box;
}

static GtkWidget *build_menu_button(main_window_t *main_window, const char *text, const char *page_name) 
{
  GtkWidget *button = create_primary_button(text, NULL, "menu-button");
  g_signal_connect(button, "clicked", G_CALLBACK(on_menu_button_clicked), main_window);

  g_object_set_data(G_OBJECT(button), "target-page", (void *)page_name);

  return button;
}

static void on_menu_button_clicked(GtkButton *button, gpointer data)
{
  main_window_t *main_window = (main_window_t *) data;

  const char *text = g_object_get_data(G_OBJECT(button), "target-page");

  gtk_stack_set_visible_child_name(main_window->stack, text);

  if (strcmp(text, "EQUIPMENT") == 0)
  {
    equipment_controller_refresh_page(&main_window->equipment_ctrl);
  }

  else if (strcmp(text, "INCIDENT") == 0)
  {
    incident_controller_refresh_page(&main_window->incident_ctrl);
  }

  else if (strcmp(text, "SENSOR") == 0)
  {
    sensor_controller_refresh_page(&main_window->sensor_ctrl);
  }

  for (int i = 0; i < PAGE_COUNT; i++) 
  {
    if (GTK_WIDGET(main_window->nav_buttons[i]) != GTK_WIDGET(button))
      gtk_toggle_button_set_active(main_window->nav_buttons[i], FALSE);
    else 
       gtk_toggle_button_set_active(main_window->nav_buttons[i], TRUE);
  }
}
