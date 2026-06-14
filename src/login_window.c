#include "login_window.h"

#include "app.h"

static void login_window_init(login_window_t *login_window, app_t *app);

static GtkWidget *build_header(login_window_t *login_window);
static GtkWidget *build_login_page(login_window_t *login_window);
static GtkWidget *build_register_page(login_window_t *login_window);


login_window_t *login_window_create(GtkApplication *gtk_app, app_t *app)
{
  login_window_t *login_window = malloc(sizeof(login_window_t));
  if (login_window == NULL) return NULL;

  login_window_init(login_window, app);

  gtk_application_add_window(gtk_app, login_window->window);

  g_object_set_data_full(G_OBJECT(login_window->window), "user-interface", login_window, free); // ownership + destroy  

  return login_window;
}

static void login_window_init(login_window_t *login_window, app_t *app)
{
  login_window->app = app;

  login_window->window = GTK_WINDOW(gtk_window_new());
  gtk_window_set_title(login_window->window, "NetPulse NOC");
  gtk_window_set_default_size(login_window->window, 1440, 900);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
  gtk_window_set_child(login_window->window, box);

  GtkWidget *header = build_header(login_window);

  login_window->stack = GTK_STACK(gtk_stack_new());

  GtkWidget *login_page = build_login_page(login_window);
  GtkWidget *register_page = build_register_page(login_window);

  gtk_stack_add_named(login_window->stack, login_page, "LOGIN");
  gtk_stack_add_named(login_window->stack, register_page, "REGISTER");

  gtk_stack_set_visible_child_name(login_window->stack, "LOGIN");
  
  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(login_window->stack));

  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, "styles/style.css");
  gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), 800);

  g_object_unref(provider);
}

static GtkWidget *build_header(login_window_t *login_window)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "header");

  GtkWidget *logo = gtk_picture_new_for_filename("assets/logo-white.svg");
  gtk_widget_set_halign(logo, GTK_ALIGN_START);
  gtk_widget_set_valign(logo, GTK_ALIGN_CENTER);
  gtk_widget_set_margin_top(logo, 194);
  //gtk_widget_set_margin_start(logo, 24);
  //gtk_widget_set_margin_bottom(logo, 4);

  gtk_box_append(GTK_BOX(box), logo);

  return box;
}

static GtkWidget *build_login_page(login_window_t *login_window)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 24);

  GtkWidget *label_username = gtk_label_new("Username");
  login_window->login_username = GTK_ENTRY(gtk_entry_new());

  GtkWidget *label_password = gtk_label_new("Password");
  login_window->login_password = GTK_ENTRY(gtk_entry_new());

  gtk_grid_attach(GTK_GRID(grid), label_username, 0, 0, 2, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_window->login_username), 0, 1, 2, 1);

  gtk_grid_attach(GTK_GRID(grid), label_password, 0, 2, 2, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_window->login_password), 0, 3, 2, 1);

  return grid;
}

static GtkWidget *build_register_page(login_window_t *login_window)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 24);

  GtkWidget *label_username = gtk_label_new("Username");
  login_window->register_username = GTK_ENTRY(gtk_entry_new());

  GtkWidget *label_password = gtk_label_new("Password");
  login_window->register_password = GTK_ENTRY(gtk_entry_new());

  gtk_grid_attach(GTK_GRID(grid), label_username, 0, 0, 2, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_window->register_username), 0, 1, 2, 1);

  gtk_grid_attach(GTK_GRID(grid), label_password, 0, 2, 2, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_window->register_password), 0, 3, 2, 1);

  return grid;
}
