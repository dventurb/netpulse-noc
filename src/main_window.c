#include "main_window.h"

#include "app.h"
#include "app_screens.h"
#include "style.h"

#include "toggle_button.h"
#include "profile_box.h"

static void main_window_destroy(void *data);
static void main_window_init(main_window_t *main_win, app_t *app);

static void build_window(main_window_t *main_win);
static void build_layout(main_window_t *main_win);
static void setup_screens(main_window_t *main_win);

static GtkWidget *build_header(main_window_t *main_win);
static GtkWidget *build_menu_bar(main_window_t *main_win);
static GtkWidget *build_menu_button(main_window_t *main_win, const screen_descriptor_t *descriptor);
static GtkWidget *build_technician_profile_box(main_window_t *main_win);

// Callbacks
static void on_menu_button_clicked(GtkButton *button, gpointer data);


main_window_t *main_window_create(GtkApplication *gtk_app, app_t *app)
{
  main_window_t *main_win = malloc(sizeof(main_window_t));
  if (main_win == NULL) return NULL;

  main_window_init(main_win, app);

  gtk_application_add_window(gtk_app, main_win->window);

  g_object_set_data_full(G_OBJECT(main_win->window), "USER-INTERFACE", main_win, main_window_destroy); // ownership + free

  return main_win;
}

static void main_window_destroy(void *data)
{
  main_window_t *main_win = (main_window_t *)data;
  if (main_win == NULL) return;

  if (main_win->screen_manager != NULL)
    screen_manager_destroy(main_win->screen_manager);

  free(main_win);
}

static void main_window_init(main_window_t *main_win, app_t *app)
{
  main_win->app = app;

  build_window(main_win);
  build_layout(main_win);
  setup_screens(main_win);
  load_styles();
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

static void setup_screens(main_window_t *main_win)
{
  main_win->screen_manager = screen_manager_create(main_win->stack, main_win->app);

  screen_manager_show(main_win->screen_manager, SCREEN_EQUIPMENT);
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

  for (int i = 0; i < SCREEN_COUNT; i++)
  {
    const screen_descriptor_t *descriptor = app_screens_get_descriptor(i);

    main_win->nav_buttons[i] = GTK_TOGGLE_BUTTON(build_menu_button(main_win, descriptor));

    gtk_box_append(GTK_BOX(box), GTK_WIDGET(main_win->nav_buttons[i]));
  }

  gtk_toggle_button_set_active(main_win->nav_buttons[SCREEN_EQUIPMENT], TRUE);
  
  return box;
}

static GtkWidget *build_menu_button(main_window_t *main_win, const screen_descriptor_t *descriptor) 
{
  GtkWidget *button = toggle_button_new(descriptor->text, NULL, "menu-button");
  g_signal_connect(button, "clicked", G_CALLBACK(on_menu_button_clicked), main_win);

  if (descriptor)
    g_object_set_data(G_OBJECT(button), "SCREEN-DESCRIPTOR", (void *)descriptor);

  return button;
}

static GtkWidget *build_technician_profile_box(main_window_t *main_win)
{
  technician_t *current_user = main_win->app->data.current_user;

  return profile_box_new(current_user->name, current_user->avatar_path);
}

static void on_menu_button_clicked(GtkButton *button, gpointer data)
{
  main_window_t *main_win = (main_window_t *) data;

  for (int i = 0; i < SCREEN_COUNT; i++) 
  {
    if (GTK_WIDGET(main_win->nav_buttons[i]) != GTK_WIDGET(button))
      gtk_toggle_button_set_active(main_win->nav_buttons[i], FALSE);
    else 
       gtk_toggle_button_set_active(main_win->nav_buttons[i], TRUE);
  }

  screen_descriptor_t *descriptor = g_object_get_data(G_OBJECT(button), "SCREEN-DESCRIPTOR");
  screen_manager_show(main_win->screen_manager, descriptor->number);
}
