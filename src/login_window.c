#include "login_window.h"

#include "app.h"
#include "auth.h"
#include "style.h"

#include "action_button.h"

static void login_window_init(login_window_t *login_win, app_t *app);

static void build_window(login_window_t *login_win);
static void build_layout(login_window_t *login_win);
static GtkWidget *build_header(void);
static GtkWidget *build_login_page(login_window_t *login_win);
static GtkWidget *build_register_page(login_window_t *login_win);
static GtkWidget *build_auth_footer(const char *lbl_text, const char *btn_text, GCallback callback, gpointer data);

static void login_clear_errors(login_window_t *login_win);
static void register_clear_errors(login_window_t *login_win);
static void login_show_error(login_window_t *login_win, login_validation_t error);
static void register_show_error(login_window_t *login_win, register_validation_t error);

// Callbacks
static void on_login_submit_clicked(GtkButton *button, gpointer data);
static void on_register_submit_clicked(GtkButton *button, gpointer data);

static void on_switch_to_register_clicked(GtkButton *button, gpointer data);
static void on_switch_to_login_clicked(GtkButton *button, gpointer data);


login_window_t *login_window_create(GtkApplication *gtk_app, app_t *app)
{
  login_window_t *login_win = malloc(sizeof(login_window_t));
  if (login_win == NULL) return NULL;

  login_window_init(login_win, app);

  gtk_application_add_window(gtk_app, login_win->window);

  g_object_set_data_full(G_OBJECT(login_win->window), "user-interface", login_win, free); // ownership + destroy  

  return login_win;
}

static void login_window_init(login_window_t *login_win, app_t *app)
{
  login_win->app = app;

  build_window(login_win);
  build_layout(login_win);
  load_styles();
}

static void build_window(login_window_t *login_win)
{
  login_win->window = GTK_WINDOW(gtk_window_new());
  gtk_window_set_title(login_win->window, "NetPulse NOC");
  gtk_window_set_default_size(login_win->window, 1440, 900);
  gtk_widget_add_css_class(GTK_WIDGET(login_win->window), "login-window");
}

static void build_layout(login_window_t *login_win)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
  gtk_window_set_child(login_win->window, container);
  gtk_widget_add_css_class(container, "login-container");
  gtk_widget_set_halign(container, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(container, GTK_ALIGN_CENTER);

  GtkWidget *header = build_header();

  login_win->stack = GTK_STACK(gtk_stack_new());
  gtk_stack_set_vhomogeneous(login_win->stack, FALSE);

  GtkWidget *login_page = build_login_page(login_win);
  GtkWidget *register_page = build_register_page(login_win);

  gtk_stack_add_named(login_win->stack, login_page, "LOGIN");
  gtk_stack_add_named(login_win->stack, register_page, "REGISTER");

  gtk_stack_set_visible_child_name(login_win->stack, "LOGIN");

  gtk_box_append(GTK_BOX(container), header);
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(login_win->stack));
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

static GtkWidget *build_login_page(login_window_t *login_win)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 24);
  gtk_widget_add_css_class(grid, "login-page");

  login_win->login_username = input_field_new("Username", "Enter your username", "assets/icon-username.svg");
  gtk_entry_set_max_length(login_win->login_username.entry, USERNAME_MAX - 1);
  gtk_widget_add_css_class(GTK_WIDGET(login_win->login_username.container), "login-field-container");

  login_win->login_password = input_field_new("Password", "••••••••", "assets/icon-password.svg");
  gtk_entry_set_max_length(login_win->login_password.entry, STRING_MAX - 1);
  gtk_entry_set_visibility(login_win->login_password.entry, FALSE);
  gtk_widget_add_css_class(GTK_WIDGET(login_win->login_password.container), "login-field-container");

  GtkWidget *login_button = action_button_new("Sign In", NULL, "secondary-button");
  gtk_widget_set_hexpand(login_button, TRUE);
  g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_submit_clicked), login_win);

  GtkWidget *footer = build_auth_footer("Don't have an account? ", "Sign up", G_CALLBACK(on_switch_to_register_clicked), login_win);

  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_win->login_username.container), 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_win->login_password.container), 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), login_button, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), footer, 0, 3, 1, 1);

  return grid;
}

static GtkWidget *build_register_page(login_window_t *login_win)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 24);
  gtk_widget_add_css_class(grid, "login-page");

  login_win->register_name = input_field_new("Full Name", "Enter your full name", NULL);
  gtk_entry_set_max_length(login_win->register_name.entry, STRING_MAX - 1);
  gtk_widget_add_css_class(GTK_WIDGET(login_win->register_name.container), "login-field-container");

  login_win->register_username = input_field_new("Username", "Enter your username", "assets/icon-username.svg");
  gtk_entry_set_max_length(login_win->register_username.entry, USERNAME_MAX - 1);
  gtk_widget_add_css_class(GTK_WIDGET(login_win->register_username.container), "login-field-container");

  login_win->register_password = input_field_new("Password", "••••••••", "assets/icon-password.svg");
  gtk_entry_set_max_length(login_win->register_password.entry, STRING_MAX - 1);
  gtk_entry_set_visibility(login_win->register_password.entry, FALSE);
  gtk_widget_add_css_class(GTK_WIDGET(login_win->register_password.container), "login-field-container");

  GtkWidget *register_button = action_button_new("Sign Up", NULL, "secondary-button");
  gtk_widget_set_hexpand(register_button, TRUE);
  g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_submit_clicked), login_win);

  GtkWidget *footer = build_auth_footer("Already have an account? ", "Log In", G_CALLBACK(on_switch_to_login_clicked), login_win);

  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_win->register_name.container), 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_win->register_username.container), 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(login_win->register_password.container), 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), register_button, 0, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), footer, 0, 4, 1, 1);

  return grid;
}

static GtkWidget *build_auth_footer(const char *lbl_text, const char *btn_text, GCallback callback, gpointer data)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(container, "login-footer-container");
  gtk_widget_set_valign(container, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(container, GTK_ALIGN_CENTER);

  GtkWidget *label = gtk_label_new(lbl_text);
  gtk_widget_add_css_class(label, "login-footer-label");

  GtkWidget *button = action_button_new(btn_text, NULL, "register-button");
  g_signal_connect(button, "clicked", G_CALLBACK(callback), data);

  gtk_box_append(GTK_BOX(container), label);
  gtk_box_append(GTK_BOX(container), button);

  return container;
}

static void login_clear_errors(login_window_t *login_win)
{
  input_field_set_error(&login_win->login_username, FALSE);
  input_field_set_error(&login_win->login_password, FALSE);
}

static void register_clear_errors(login_window_t *login_win)
{
  input_field_set_error(&login_win->register_name, FALSE);
  input_field_set_error(&login_win->register_username, FALSE);
  input_field_set_error(&login_win->register_password, FALSE);
}

static void login_show_error(login_window_t *login_win, login_validation_t error)
{
  login_clear_errors(login_win);

  switch (error) 
  {
    case LOGIN_INVALID_USERNAME:
      input_field_set_error(&login_win->login_username, TRUE);
      break;

    case LOGIN_INVALID_PASSWORD:
      input_field_set_error(&login_win->login_password, TRUE);
      break;

    case LOGIN_SUCCESSFUL: break;
  }
}

static void register_show_error(login_window_t *login_win, register_validation_t error)
{
  register_clear_errors(login_win);

  switch (error) 
  {
    case REGISTER_INVALID_NAME:
      input_field_set_error(&login_win->register_name, TRUE);
      break;

    case REGISTER_INVALID_USERNAME:
      input_field_set_error(&login_win->register_username, TRUE);
      break;

    case REGISTER_INVALID_PASSWORD:
      input_field_set_error(&login_win->register_password, TRUE);
      break;

    case REGISTER_VALID: break;
  }
}

static void on_switch_to_register_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  login_window_t *login_win = (login_window_t *)data;

  gtk_stack_set_visible_child_name(login_win->stack, "REGISTER");
}

static void on_switch_to_login_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  login_window_t *login_win = (login_window_t *)data;

  gtk_stack_set_visible_child_name(login_win->stack, "LOGIN");
}

static void on_login_submit_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  login_window_t *login_win = (login_window_t *)data;

  const char *username = gtk_editable_get_text(GTK_EDITABLE(login_win->login_username.entry));
  const char *password = gtk_editable_get_text(GTK_EDITABLE(login_win->login_password.entry));

  login_validation_t error = auth_login(login_win->app, username, password);

  if (error != LOGIN_SUCCESSFUL)
  {
    login_show_error(login_win, error);
    return;
  }

  login_clear_errors(login_win);
  app_windows_switch(&login_win->app->windows, login_win->app);
}

static void on_register_submit_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  login_window_t *login_win = (login_window_t *)data;

  const char *name = gtk_editable_get_text(GTK_EDITABLE(login_win->register_name.entry));
  const char *username = gtk_editable_get_text(GTK_EDITABLE(login_win->register_username.entry));
  const char *password = gtk_editable_get_text(GTK_EDITABLE(login_win->register_password.entry));

  register_validation_t error = auth_validate_register(login_win->app, name, username, password);

  if (error != REGISTER_VALID)
  {
    register_show_error(login_win, error);
    return;
  }

  register_clear_errors(login_win);

  bool is_registered = auth_register_new_technician(login_win->app, name, username, password);

  if (is_registered)
    app_windows_switch(&login_win->app->windows, login_win->app);
}
