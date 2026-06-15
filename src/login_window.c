#include "login_window.h"

#include "auth.h"
#include "app.h"

static void login_window_init(login_window_t *login_window, app_t *app);

static GtkWidget *build_header(void);
static GtkWidget *build_login_page(login_window_t *login_window);
static GtkWidget *build_register_page(login_window_t *login_window);

// Callbacks
static void on_login_submit_clicked(GtkButton *button, gpointer data);
static void on_register_submit_clicked(GtkButton *button, gpointer data);

static void on_switch_to_register_clicked(GtkButton *button, gpointer data);
static void on_switch_to_login_clicked(GtkButton *button, gpointer data);


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
  gtk_widget_add_css_class(GTK_WIDGET(login_window->window), "login-window");

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
  gtk_window_set_child(login_window->window, box);
  gtk_widget_add_css_class(box, "login-container");
  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

  GtkWidget *header = build_header();

  login_window->stack = GTK_STACK(gtk_stack_new());
  gtk_stack_set_vhomogeneous(login_window->stack, FALSE);

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

static GtkWidget *build_header(void)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "login-header");
  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

  GtkWidget *logo = gtk_picture_new_for_filename("assets/logo-white.svg");
  gtk_widget_set_halign(logo, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(logo, GTK_ALIGN_CENTER);
  //gtk_widget_set_margin_top(logo, 190);
  gtk_widget_set_margin_start(logo, 40);
  gtk_widget_set_margin_bottom(logo, 20);
  gtk_widget_set_size_request(logo, 64, 64);

  gtk_box_append(GTK_BOX(box), logo);

  return box;
}

static GtkWidget *build_login_page(login_window_t *login_window)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 24);
  gtk_widget_add_css_class(grid, "login-page");

  GtkWidget *label_username = gtk_label_new("Username");
  gtk_widget_set_halign(label_username, GTK_ALIGN_START);
  gtk_widget_add_css_class(label_username, "login-label");

  login_window->login_username = GTK_ENTRY(gtk_entry_new());
  gtk_widget_add_css_class(GTK_WIDGET(login_window->login_username), "login-entry");
  gtk_widget_set_size_request(GTK_WIDGET(login_window->login_username), 336, 40);

  GtkWidget *label_password = gtk_label_new("Password");
  gtk_widget_set_halign(label_password, GTK_ALIGN_START);
  gtk_widget_add_css_class(label_password, "login-label");

  login_window->login_password = GTK_ENTRY(gtk_entry_new());
  gtk_widget_add_css_class(GTK_WIDGET(login_window->login_password), "login-entry");
  gtk_entry_set_visibility(login_window->login_password, FALSE);
  gtk_widget_set_size_request(GTK_WIDGET(login_window->login_password), 336, 40);

  GtkWidget *login_button = create_secondary_button("Sign In", NULL, "secondary-button");
  gtk_widget_set_hexpand(login_button, TRUE);
  g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_submit_clicked), login_window);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "login-footer-box");
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);

  GtkWidget *label_technician = gtk_label_new("Don't have an account? ");
  gtk_widget_add_css_class(label_technician, "login-label");

  GtkWidget *register_button = create_secondary_button("Sing up", NULL, "register-button");
  g_signal_connect(register_button, "clicked", G_CALLBACK(on_switch_to_register_clicked), login_window);

  gtk_box_append(GTK_BOX(box), label_technician);
  gtk_box_append(GTK_BOX(box), register_button);

  gtk_grid_attach(GTK_GRID(grid), label_username, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_window->login_username), 0, 1, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), label_password, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_window->login_password), 0, 3, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), login_button, 0, 4, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), box, 0, 5, 1, 1);

  return grid;
}

static GtkWidget *build_register_page(login_window_t *login_window)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 24);
  gtk_widget_add_css_class(grid, "login-page");

  GtkWidget *label_name = gtk_label_new("Name");
  gtk_widget_set_halign(label_name, GTK_ALIGN_START);
  gtk_widget_add_css_class(label_name, "login-label");

  login_window->register_name = GTK_ENTRY(gtk_entry_new());
  gtk_widget_add_css_class(GTK_WIDGET(login_window->register_name), "login-entry");
  gtk_widget_set_size_request(GTK_WIDGET(login_window->register_name), 336, 40);

  GtkWidget *label_username = gtk_label_new("Username");
  gtk_widget_set_halign(label_username, GTK_ALIGN_START);
  gtk_widget_add_css_class(label_username, "login-label");

  login_window->register_username = GTK_ENTRY(gtk_entry_new());
  gtk_widget_add_css_class(GTK_WIDGET(login_window->register_username), "login-entry");
  gtk_widget_set_size_request(GTK_WIDGET(login_window->register_username), 336, 40);

  GtkWidget *label_password = gtk_label_new("Password");
  gtk_widget_set_halign(label_password, GTK_ALIGN_START);
  gtk_widget_add_css_class(label_password, "login-label");

  login_window->register_password = GTK_ENTRY(gtk_entry_new());
  gtk_widget_add_css_class(GTK_WIDGET(login_window->register_password), "login-entry");
  gtk_entry_set_visibility(login_window->register_password, FALSE);
  gtk_widget_set_size_request(GTK_WIDGET(login_window->register_password), 336, 40);

  GtkWidget *register_button = create_secondary_button("Sign Up", NULL, "secondary-button");
  gtk_widget_set_hexpand(register_button, TRUE);
  g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_submit_clicked), login_window);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "login-footer-box");
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);

  GtkWidget *label_technician = gtk_label_new("Already have an account? ");
  gtk_widget_add_css_class(label_technician, "login-label");

  GtkWidget *login_button = create_secondary_button("Log In", NULL, "register-button");
  g_signal_connect(login_button, "clicked", G_CALLBACK(on_switch_to_login_clicked), login_window);

  gtk_box_append(GTK_BOX(box), label_technician);
  gtk_box_append(GTK_BOX(box), login_button);

  gtk_grid_attach(GTK_GRID(grid), label_name, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_window->register_name), 0, 1, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), label_username, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_window->register_username), 0, 3, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), label_password, 0, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_window->register_password), 0, 5, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), register_button, 0, 6, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), box, 0, 7, 1, 1);

  return grid;
}

static void on_switch_to_register_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  login_window_t *login_window = (login_window_t *)data;

  gtk_stack_set_visible_child_name(login_window->stack, "REGISTER");
}

static void on_switch_to_login_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  login_window_t *login_window = (login_window_t *)data;

  gtk_stack_set_visible_child_name(login_window->stack, "LOGIN");
}


static void on_login_submit_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  login_window_t *login_window = (login_window_t *)data;

  const char *username = gtk_editable_get_text(GTK_EDITABLE(login_window->login_username));
  const char *password = gtk_editable_get_text(GTK_EDITABLE(login_window->login_password));

  gtk_widget_remove_css_class(GTK_WIDGET(login_window->login_username), "login-entry-error");
  gtk_widget_remove_css_class(GTK_WIDGET(login_window->login_password), "login-entry-error");

  login_validation_t error = auth_login(login_window->app, username, password);

  switch (error) 
  {
    case LOGIN_INVALID_USERNAME:
      gtk_widget_add_css_class(GTK_WIDGET(login_window->login_username), "login-entry-error");
      return;
    case LOGIN_INVALID_PASSWORD:
      gtk_widget_add_css_class(GTK_WIDGET(login_window->login_password), "login-entry-error");
      return;
    case LOGIN_SUCCESSFUL:
      app_windows_switch(&login_window->app->windows, login_window->app);
      break;
  }
}

static void on_register_submit_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  login_window_t *login_window = (login_window_t *)data;

  const char *name = gtk_editable_get_text(GTK_EDITABLE(login_window->register_name));
  const char *username = gtk_editable_get_text(GTK_EDITABLE(login_window->register_username));
  const char *password = gtk_editable_get_text(GTK_EDITABLE(login_window->register_password));

  gtk_widget_remove_css_class(GTK_WIDGET(login_window->register_name), "login-entry-error");
  gtk_widget_remove_css_class(GTK_WIDGET(login_window->register_username), "login-entry-error");
  gtk_widget_remove_css_class(GTK_WIDGET(login_window->register_password), "login-entry-error");

  register_validation_t error = auth_validate_register(login_window->app, name, username, password);

  switch (error) 
  {
    case REGISTER_INVALID_NAME:
      gtk_widget_add_css_class(GTK_WIDGET(login_window->register_name), "login-entry-error");
      return;
    case REGISTER_INVALID_USERNAME:
      gtk_widget_add_css_class(GTK_WIDGET(login_window->register_username), "login-entry-error");
      return;
    case REGISTER_INVALID_PASSWORD:
      gtk_widget_add_css_class(GTK_WIDGET(login_window->register_password), "login-entry-error");
      return;
    case REGISTER_VALID: break;
  }

  bool is_registered = auth_register_new_technician(login_window->app, name, username, password);

  if (is_registered == FALSE) return;
  else  gtk_stack_set_visible_child_name(login_window->stack, "LOGIN");
}
