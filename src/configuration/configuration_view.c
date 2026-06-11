#include "configuration_view.h"

#include "utils.h"
#include "ui_widgets.h"
#include "macros.h"
#include "ui_macros.h"
#include "pagination.h"

static const char* const headers[] = { "#", "COMMAND", "TECHNICIAN", "DATE/TIME" };
static const int widths[] = { CELL_CODE_WIDTH, CELL_TYPE_WIDTH, CELL_VALUE_WIDTH, CELL_STATUS_WIDTH };

static const char* const DATA_EQUIPMENT_ID = "equipment-id";

static const int CONFIG_HEADER_COLUMN_COUNT = 4;
static const int CONFIG_TABLE_COLUMN_COUNT = 4;

static GtkWidget *build_sidebar(configuration_view_t *view);
static GtkWidget *build_content(configuration_view_t *view);
static GtkWidget *build_header(configuration_view_t *view);
static GtkWidget *build_stats_cards(configuration_view_t *view);
static GtkWidget *build_pagination_bar(configuration_view_t *view);

static GtkWidget *build_config_table(configuration_view_t *view);
static void build_config_table_header(GtkWidget *table);
static void build_config_table_row(configuration_view_t *view, configuration_t sensor, int row);

static GtkWidget *build_equipment_list(configuration_view_t *view);
static void build_equipment_list_row(GtkWidget *list, equipment_t equipment);
static GtkWidget *build_equipment_cell(equipment_t equipment);

static GtkWidget *build_add_config_form(configuration_view_t *view);

// Callbacks
static void on_add_config_clicked(GtkButton *button, gpointer data);
static void on_add_config_form_submit(GtkButton *button, gpointer data);

static void on_search_equipment_clicked(GtkSearchEntry *search, gpointer data);
static void on_equipment_row_selected(GtkListBox *list, GtkListBoxRow *row, gpointer data);

static void on_previous_page_clicked(GtkButton *button, gpointer data);
static void on_next_page_clicked(GtkButton *button, gpointer data);
static void on_page_clicked(GtkButton *button, gpointer data);


GtkBox *configuration_view_create(configuration_view_t *view, configuration_controller_t *controller)
{
  view->controller = controller;

  view->container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

  GtkWidget *side_bar = build_sidebar(view);
  GtkWidget *content = build_content(view);

  gtk_box_append(view->container, side_bar);
  gtk_box_append(view->container, content);

  return view->container;
}

void configuration_view_refresh(configuration_view_t *view)
{
  configuration_controller_reset_config_query(view->controller);
  configuration_controller_reset_equipment_query(view->controller);
}

void configuration_view_update(configuration_view_t *view)
{
  configuration_controller_start_config_query(view->controller);
}

void configuration_view_update_cards(configuration_view_t *view)
{
  remove_all_children_from_widget(GTK_WIDGET(view->cards));

  //sensor_stats_t stats = {0};

  //configuration_controller_get_stats(view->controller, &stats);

  GtkWidget *total_card = create_stats_card("Total Sensors", 0, "default-card");
  GtkWidget *ok_card = create_stats_card("OK", 0, "operational-card");
  GtkWidget *warning_card = create_stats_card("Warning + Critical", 0, "maintenance-card");
  GtkWidget *failure_card = create_stats_card("Network Failures", 0, "failed-card");

  gtk_box_append(view->cards, total_card);
  gtk_box_append(view->cards, ok_card);
  gtk_box_append(view->cards, warning_card);
  gtk_box_append(view->cards, failure_card);
}

void configuration_view_update_config_table(configuration_view_t *view, const void *result, int count)
{
  remove_table_rows(GTK_WIDGET(view->table));

  if (result == NULL || count == 0) return;

  configuration_t *configs = (configuration_t *)result;

  for (int i = 0; i < count; i++) 
  {
    build_config_table_row(view, configs[i], i + 1);
  }

  GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(view->pagination_bar));
  
  if (parent != NULL)
  {
    gtk_box_remove(GTK_BOX(parent), GTK_WIDGET(view->pagination_bar));

    view->pagination_bar = GTK_BOX(build_pagination_bar(view));
    gtk_box_append(GTK_BOX(parent), GTK_WIDGET(view->pagination_bar));
  }
}

void configuration_view_update_equipment_list(configuration_view_t *view, const void *result, int count)
{
  remove_list_rows(GTK_WIDGET(view->list));

  if (result == NULL || count == 0) return;

  equipment_t *equipments = (equipment_t *)result;

  for (int i = 0; i < count; i++)
  {
    build_equipment_list_row(GTK_WIDGET(view->list), equipments[i]);
  }
}

void configuration_view_set_actions_enabled(configuration_view_t *view, bool is_active)
{
  gtk_widget_set_sensitive(GTK_WIDGET(view->add_button), is_active);
  gtk_widget_set_sensitive(GTK_WIDGET(view->revert_button), is_active);
  gtk_widget_set_sensitive(GTK_WIDGET(view->clear_button), is_active);
}

static GtkWidget *build_sidebar(configuration_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 280, -1);
  gtk_widget_add_css_class(box, "sidebar");

  GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_margin_start(sidebar, 14);
  gtk_widget_set_margin_end(sidebar, 14);
  gtk_widget_set_margin_top(sidebar, 24);

  GtkWidget *title = gtk_label_new("Equipment List");
  gtk_widget_add_css_class(title, "sidebar-title");
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  GtkWidget *search = gtk_search_entry_new();
  gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search), "Search equipments...");
  gtk_widget_add_css_class(search, "sidebar-search");
  g_signal_connect(search, "search-changed", G_CALLBACK(on_search_equipment_clicked), view);
  
  GtkWidget *list = build_equipment_list(view);
  
  gtk_box_append(GTK_BOX(sidebar), title);
  gtk_box_append(GTK_BOX(sidebar), search);
  gtk_box_append(GTK_BOX(sidebar), list);

  gtk_box_append(GTK_BOX(box), sidebar);

  return box;
}

static GtkWidget *build_content(configuration_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_set_margin_start(box, 24);
  gtk_widget_set_margin_end(box, 24);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);
  //gtk_widget_set_size_request(box, 1160, -1);

  GtkWidget *header = build_header(view);
  view->cards = GTK_BOX(build_stats_cards(view));
  GtkWidget *table = build_config_table(view);

  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->cards));
  gtk_box_append(GTK_BOX(box), table);

  return box;
}

static GtkWidget *build_header(configuration_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

  GtkWidget *title = gtk_label_new("Configuration History");
  gtk_widget_add_css_class(title, "inventory-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  view->revert_button = GTK_BUTTON(create_secondary_button("Revert Last", "assets/icon-revert.svg", "revert-button"));
  //g_signal_connect(GTK_WIDGET(view->import_button), "clicked", G_CALLBACK(on_import_sensors_clicked), view);

  view->clear_button = GTK_BUTTON(create_secondary_button("Clear History", "assets/icon-clear.svg", "clear-button"));
  //g_signal_connect(GTK_WIDGET(view->fetch_button), "clicked", G_CALLBACK(on_fetch_api_clicked), view);

  view->add_button = GTK_BUTTON(create_secondary_button("Add Configuration", "assets/icon-add.svg", "secondary-button"));
  g_signal_connect(GTK_WIDGET(view->add_button), "clicked", G_CALLBACK(on_add_config_clicked), view);

  gtk_box_append(GTK_BOX(box), title);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->revert_button));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->clear_button));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->add_button));

  return box;
}

static GtkWidget *build_stats_cards(configuration_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(box, TRUE);

  //configuration_stats_t stats = {0};

  //configuration_controller_get_stats(view->controller, &stats);

  GtkWidget *total_card = create_stats_card("Total Sensors", 0, "default-card");
  GtkWidget *ok_card = create_stats_card("OK", 0, "operational-card");
  GtkWidget *warning_card = create_stats_card("Warning + Critical", 0, "maintenance-card");
  GtkWidget *failure_card = create_stats_card("Network Failures", 0, "failed-card");

  gtk_box_append(GTK_BOX(box), total_card);
  gtk_box_append(GTK_BOX(box), ok_card);
  gtk_box_append(GTK_BOX(box), warning_card);
  gtk_box_append(GTK_BOX(box), failure_card);

  return box;
}

static GtkWidget *build_equipment_list(configuration_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  GtkWidget *scrolled_window = gtk_scrolled_window_new();
  gtk_widget_set_vexpand(scrolled_window, TRUE);
  //gtk_widget_set_size_request(scrolled_window, -1, 456);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_widget_add_css_class(scrolled_window, "table-scroll");

  view->list = GTK_LIST_BOX(gtk_list_box_new());
  g_signal_connect(GTK_WIDGET(view->list), "row-selected", G_CALLBACK(on_equipment_row_selected), view);

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(view->list));

  gtk_box_append(GTK_BOX(box), scrolled_window);

  configuration_controller_start_equipment_query(view->controller);

  return box;
}

static GtkWidget *build_config_table(configuration_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_add_css_class(box, "inventory");

  GtkWidget *scrolled_window = gtk_scrolled_window_new();
  gtk_widget_set_size_request(scrolled_window, -1, 456);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
  gtk_widget_add_css_class(scrolled_window, "table-scroll");

  view->table = GTK_GRID(gtk_grid_new());
  gtk_widget_set_hexpand(GTK_WIDGET(view->table), FALSE);
  gtk_widget_set_halign(GTK_WIDGET(view->table), GTK_ALIGN_FILL);
  gtk_widget_add_css_class(GTK_WIDGET(view->table), "table");

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(view->table));

  build_config_table_header(GTK_WIDGET(view->table));

  view->pagination_bar = GTK_BOX(build_pagination_bar(view));

  gtk_box_append(GTK_BOX(box), scrolled_window);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->pagination_bar));

  configuration_controller_reset_config_query(view->controller);

  return box;
}

static void build_equipment_list_row(GtkWidget *list, equipment_t equipment)
{
  GtkWidget *item = build_equipment_cell(equipment);

  GtkWidget *row = gtk_list_box_row_new();

  char buffer[ID_MAX];
  equipment_format_id(equipment.id, buffer);

  char *equipment_id = strdup(buffer);
  g_object_set_data_full(G_OBJECT(row), DATA_EQUIPMENT_ID, equipment_id, free); // ownership + free automatic

  gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), item);
  gtk_widget_add_css_class(row, "list-equipment-item");

  gtk_list_box_append(GTK_LIST_BOX(list), row);
}

static GtkWidget *build_equipment_cell(equipment_t equipment) 
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

  GtkWidget *image;

  switch (equipment.status) 
  {
    case STATUS_FAILED:
      image = gtk_image_new_from_file("assets/status-failed.svg");
      break;
    case STATUS_MAINTENANCE:
      image = gtk_image_new_from_file("assets/status-maintenance.svg");
      break;
    case STATUS_OPERATIONAL:
      image = gtk_image_new_from_file("assets/status-operational.svg");
      break;
    case STATUS_DISABLED:
      image = gtk_image_new_from_file("assets/status-disabled.svg");
      break;
  }

  gtk_image_set_pixel_size(GTK_IMAGE(image), 8);

  GtkWidget *text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_widget_set_hexpand(text_box, TRUE);

  GtkWidget *name = gtk_label_new(equipment.name);
  gtk_widget_set_halign(name, GTK_ALIGN_START);
  gtk_widget_add_css_class(name, "list-equipment-name");

  char buffer[ID_MAX];
  equipment_format_id(equipment.id, buffer);

  GtkWidget *id = gtk_label_new(buffer);
  gtk_widget_set_halign(id, GTK_ALIGN_START);
  gtk_widget_add_css_class(id, "list-equipment-id");

  gtk_box_append(GTK_BOX(text_box), name);
  gtk_box_append(GTK_BOX(text_box), id);

  gtk_box_append(GTK_BOX(box), text_box);
  gtk_box_append(GTK_BOX(box), image);

  return box;
}

static GtkWidget *build_add_config_form(configuration_view_t *view)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_widget_set_size_request(grid, 500, 268);
  gtk_widget_set_margin_start(grid, 24);
  gtk_widget_set_margin_end(grid, 24);
  gtk_widget_set_margin_top(grid, 24);
  gtk_widget_set_margin_bottom(grid, 40);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 24);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 24);
  gtk_widget_add_css_class(grid, "dialog-form");

  equipment_t *equipment = configuration_controller_get_selected_equipment(view->controller);

  view->form.entry_equipment = GTK_ENTRY(create_text_field(grid, "EQUIPMENT", equipment->name, 0, 0));
  gtk_widget_add_css_class(GTK_WIDGET(view->form.entry_equipment), "form-entry-disabled");
  gtk_editable_set_editable(GTK_EDITABLE(view->form.entry_equipment), FALSE);

  view->form.entry_command = GTK_ENTRY(create_text_field(grid, "CONFIGURATION COMMAND", "e.g. interface gigabitethernet 0/1",  1, 0));
  gtk_entry_set_max_length(view->form.entry_command, COMMAND_MAX - 1);

  return grid;
}

static GtkWidget *build_pagination_bar(configuration_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  gtk_widget_set_size_request(box, -1, 64);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_set_halign(box, GTK_ALIGN_END);

  GtkWidget *previous_button = create_secondary_button(NULL, "assets/left-arrow.svg", "arrow-page");
  gtk_widget_set_margin_top(previous_button, 16);
  gtk_widget_set_margin_bottom(previous_button, 16);
  gtk_widget_set_size_request(previous_button, 32, 32);
  
  g_signal_connect(previous_button, "clicked", G_CALLBACK(on_previous_page_clicked), view);
  gtk_box_append(GTK_BOX(box), previous_button);

  int start, end;

  pagination_get_range(view->controller->pagination, &start, &end);

  for (int i = start; i <= end; i++) 
  {
    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%d", i + 1);

    GtkWidget *button = create_pagination_button(view->controller->pagination, buffer, i);
    g_signal_connect(button, "clicked", G_CALLBACK(on_page_clicked), view);

    gtk_box_append(GTK_BOX(box), button);
  }

  GtkWidget *next_button = create_secondary_button(NULL, "assets/right-arrow.svg", "arrow-page");
  gtk_widget_set_margin_top(next_button, 16);
  gtk_widget_set_margin_bottom(next_button, 16);
  gtk_widget_set_margin_end(next_button, 24);
  g_signal_connect(next_button, "clicked", G_CALLBACK(on_next_page_clicked), view);
  gtk_widget_set_size_request(next_button, 32, 32);
 
  gtk_box_append(GTK_BOX(box), next_button);

  return box;
}

static void build_config_table_header(GtkWidget *table)
{
  for (int i = 0; i < CONFIG_HEADER_COLUMN_COUNT; i++) {
    GtkWidget *header_col = create_table_header(headers[i], widths[i]);
    gtk_widget_set_hexpand(header_col, TRUE);
    gtk_grid_attach(GTK_GRID(table), header_col, i, 0, 1, 1);
  }
}

static void build_config_table_row(configuration_view_t *view, configuration_t config, int row)
{
  const char *css_class = (row % 2 == 0) ? "table-row-even" : "table-row-odd";
 
  char number[NUMBER_MAX];
  snprintf(number, NUMBER_MAX, "%d", config.number);

  char datetime[DATETIME_MAX];
  get_datetime(config.operated_at, datetime);

  char name[100];
  snprintf(name, 100, "-");

  GtkWidget *columns[] = {
    create_table_cell(number, CELL_CODE_WIDTH),
    create_table_cell(config.command, CELL_TYPE_WIDTH),
    create_table_cell(name, CELL_VALUE_WIDTH),
    create_table_cell(datetime, CELL_READ_AT_WIDTH)
  };

  for (int i = 0; i < CONFIG_TABLE_COLUMN_COUNT; i++) {
    
    gtk_widget_add_css_class(columns[i], css_class);
    gtk_widget_set_hexpand(columns[i], TRUE);

    gtk_grid_attach(view->table, columns[i], i, row, 1, 1);
  }
}

static void on_add_config_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused 
  
  configuration_view_t *view = (configuration_view_t *)data;

  if (!configuration_controller_has_selected_equipment(view->controller)) return;

  view->form.layout = build_add_config_form(view);

  GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(view->container)));

  dialog_config_t config =
  {
    .window = GTK_WIDGET(window),
    .form = view->form.layout,
    .title = "Add Configuration",
    .dialog_action = 
    {
      .label = "Apply Configuration",
      .icon = "assets/icon-add.svg",
      .css = "dialog-footer-add-button",
      .callback = G_CALLBACK(on_add_config_form_submit),
      .data = view
    }
  };

  view->form.dialog = GTK_WINDOW(create_dialog_window(config));

  gtk_window_present(view->form.dialog);
}

static void on_add_config_form_submit(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter

  configuration_view_t *view = (configuration_view_t *)data;

  configuration_t new = {0};
  new.technician_name[0] = '\0'; // TODO

  GtkEntry *entry = view->form.entry_command;
  const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
  snprintf(new.command, COMMAND_MAX, "%s", text);

  if (!configuration_controller_validate(new))
  {
    gtk_widget_add_css_class(GTK_WIDGET(entry), "form-entry-error");
    return;
  }

  configuration_controller_add(view->controller, new);

  gtk_window_destroy(view->form.dialog);
}

static void on_search_equipment_clicked(GtkSearchEntry *search, gpointer data)
{
  configuration_view_t *view = (configuration_view_t *)data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(search));

  configuration_controller_set_search(view->controller, text);
}

static void on_equipment_row_selected(GtkListBox *list, GtkListBoxRow *row, gpointer data)
{
  (void)list; // unused 

  if (row == NULL) return;
  
  configuration_view_t *view = (configuration_view_t *)data;

  const char *equipment_id = g_object_get_data(G_OBJECT(row), DATA_EQUIPMENT_ID);

  configuration_controller_set_selected_equipment(view->controller, equipment_id);

  configuration_view_update(view);
}

static void on_previous_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  configuration_view_t *view = (configuration_view_t *)data;

  pagination_previous(&view->controller->pagination);

  configuration_view_update(view);
}

static void on_next_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter 

  configuration_view_t *view = (configuration_view_t *)data;

  pagination_next(&view->controller->pagination);

  configuration_view_update(view);
}

static void on_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter 
  
  configuration_view_t *view = (configuration_view_t *)data;

  int page_number = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "page-number"));

  pagination_set_page_number(&view->controller->pagination, page_number);

  configuration_view_update(view);
}
