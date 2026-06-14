#include "sensor_view.h"

#include "utils.h"
#include "ui_widgets.h"
#include "macros.h"
#include "pagination.h"

static const char* const filter_status[] = { "All", "OK", "Warning", "Critical", "Network Failure", NULL };
static const char* const headers[] = { "CODE", "TYPE", "VALUE", "STATUS", "READ AT" };
static const int widths[] = { CELL_CODE_WIDTH, CELL_TYPE_WIDTH, CELL_VALUE_WIDTH, CELL_STATUS_WIDTH, CELL_READ_AT_WIDTH };

static const int SENSOR_HEADER_COLUMN_COUNT = 5;
static const int SENSOR_TABLE_COLUMN_COUNT = 5;

static GtkWidget *build_sidebar(sensor_view_t *view);
static GtkWidget *build_content(sensor_view_t *view);
static GtkWidget *build_header(sensor_view_t *view);
static GtkWidget *build_stats_cards(sensor_view_t *view);
static GtkWidget *build_filter_bar(sensor_view_t *view);
static GtkWidget *build_table(sensor_view_t *view);
static GtkWidget *build_pagination_bar(sensor_view_t *view);

static void build_table_header(GtkWidget *table);
static void build_table_row(sensor_view_t *view, sensor_t sensor, int row);
static GtkWidget *build_status_cell(sensor_status_t status);

static void sensor_view_apply_filters(sensor_view_t *view);

// Callbacks
static void on_import_sensors_clicked(GtkButton *button, gpointer data);
static void on_fetch_api_clicked(GtkButton *button, gpointer data);

static void on_search_entry_changed(GtkSearchEntry *search, gpointer data);
static void on_date_entry_changed(GtkEntry *entry, gpointer data);
static void on_filter_dropdown_changed(GObject *self, GParamSpec *pspec, gpointer data);

static void on_previous_page_clicked(GtkButton *button, gpointer data);
static void on_next_page_clicked(GtkButton *button, gpointer data);
static void on_page_clicked(GtkButton *button, gpointer data);


GtkBox *sensor_view_create(sensor_view_t *view, sensor_controller_t *controller)
{
  view->controller = controller;

  view->container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

  GtkWidget *side_bar = build_sidebar(view);
  GtkWidget *content = build_content(view);

  gtk_box_append(view->container, side_bar);
  gtk_box_append(view->container, content);

  return view->container;
}

void sensor_view_refresh(sensor_view_t *view)
{
  sensor_controller_reset_query(view->controller);
}

void sensor_view_update(sensor_view_t *view)
{
  sensor_controller_start_query(view->controller);
}

void sensor_view_update_stats_cards(sensor_view_t *view)
{
  remove_all_children_from_widget(GTK_WIDGET(view->cards));

  sensor_stats_t stats = {0};

  sensor_controller_get_stats(view->controller, &stats);

  GtkWidget *total_card = create_stats_card("Total Sensors", stats.total, "default-card");
  GtkWidget *ok_card = create_stats_card("OK", stats.ok, "operational-card");
  GtkWidget *warning_card = create_stats_card("Warning + Critical", stats.warning + stats.critical, "maintenance-card");
  GtkWidget *failure_card = create_stats_card("Network Failures", stats.failure, "failed-card");

  gtk_box_append(view->cards, total_card);
  gtk_box_append(view->cards, ok_card);
  gtk_box_append(view->cards, warning_card);
  gtk_box_append(view->cards, failure_card);
}

void sensor_view_update_table(sensor_view_t *view, const sensor_t *sensors, int count)
{
  remove_table_rows(GTK_WIDGET(view->table));

  if (sensors == NULL || count == 0) return;

  for (int i = 0; i < count; i++) 
  {
    build_table_row(view, sensors[i], i + 1);
  }

  GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(view->pagination_bar));
  
  if (parent != NULL)
  {
    gtk_box_remove(GTK_BOX(parent), GTK_WIDGET(view->pagination_bar));

    view->pagination_bar = GTK_BOX(build_pagination_bar(view));
    gtk_box_append(GTK_BOX(parent), GTK_WIDGET(view->pagination_bar));
  }
}

void sensor_view_set_actions_enabled(sensor_view_t *view, bool is_active)
{
  gtk_widget_set_sensitive(GTK_WIDGET(view->import_button), is_active);
  gtk_widget_set_sensitive(GTK_WIDGET(view->fetch_button), is_active);
}

static GtkWidget *build_sidebar(sensor_view_t *view)
{
  (void)view; // unused parameter

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 280, -1);

  return box;
}

static GtkWidget *build_content(sensor_view_t *view)
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
  GtkWidget *filters = build_filter_bar(view);
  GtkWidget *table = build_table(view);

  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->cards));
  gtk_box_append(GTK_BOX(box), filters);
  gtk_box_append(GTK_BOX(box), table);

  return box;
}

static GtkWidget *build_header(sensor_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

  GtkWidget *title = gtk_label_new("Sensors Monitoring");
  gtk_widget_add_css_class(title, "inventory-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  view->import_button = GTK_BUTTON(create_secondary_button("Import Sensors", "assets/icon-import.svg", "import-button"));
  g_signal_connect(GTK_WIDGET(view->import_button), "clicked", G_CALLBACK(on_import_sensors_clicked), view);

  view->fetch_button = GTK_BUTTON(create_secondary_button("Fetch API", "assets/icon-fetch.svg", "fetch-button"));
  g_signal_connect(GTK_WIDGET(view->fetch_button), "clicked", G_CALLBACK(on_fetch_api_clicked), view);

  gtk_box_append(GTK_BOX(box), title);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->import_button));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->fetch_button));

  return box;
}

static GtkWidget *build_stats_cards(sensor_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(box, TRUE);

  sensor_stats_t stats = {0};

  sensor_controller_get_stats(view->controller, &stats);

  GtkWidget *total_card = create_stats_card("Total Sensors", stats.total, "default-card");
  GtkWidget *ok_card = create_stats_card("OK", stats.ok, "operational-card");
  GtkWidget *warning_card = create_stats_card("Warning + Critical", stats.warning + stats.critical, "maintenance-card");
  GtkWidget *failure_card = create_stats_card("Network Failures", stats.failure, "failed-card");

  gtk_box_append(GTK_BOX(box), total_card);
  gtk_box_append(GTK_BOX(box), ok_card);
  gtk_box_append(GTK_BOX(box), warning_card);
  gtk_box_append(GTK_BOX(box), failure_card);

  return box;
}

static GtkWidget *build_filter_bar(sensor_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(box, TRUE);

  GtkWidget *search = gtk_search_entry_new();
  gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search), "Search by sensor code...");
  gtk_widget_add_css_class(search, "inventory-search");
  gtk_widget_set_hexpand(search, TRUE);
  g_signal_connect(search, "search-changed", G_CALLBACK(on_search_entry_changed), view);

  view->date_filter = GTK_ENTRY(gtk_entry_new());
  gtk_entry_set_placeholder_text(view->date_filter, "DD-MM-AAAA");
  gtk_entry_set_max_length(view->date_filter, DATE_MAX - 1);
  gtk_widget_add_css_class(GTK_WIDGET(view->date_filter), "date-entry");
  g_signal_connect(view->date_filter, "activate", G_CALLBACK(on_date_entry_changed), view);

  char date[DATE_MAX];
  format_current_date(date);
  gtk_editable_set_text(GTK_EDITABLE(view->date_filter), date);

  view->status_filter = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(filter_status));
  gtk_widget_add_css_class(GTK_WIDGET(view->status_filter), "inventory-filter");
  g_signal_connect(GTK_WIDGET(view->status_filter), "notify::selected", G_CALLBACK(on_filter_dropdown_changed), view);

  gtk_box_append(GTK_BOX(box), search);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->date_filter));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->status_filter));

  return box;
}

static GtkWidget *build_table(sensor_view_t *view)
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

  build_table_header(GTK_WIDGET(view->table));

  view->pagination_bar = GTK_BOX(build_pagination_bar(view));

  gtk_box_append(GTK_BOX(box), scrolled_window);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->pagination_bar));

  sensor_controller_reset_query(view->controller);

  return box;
}

static GtkWidget *build_pagination_bar(sensor_view_t *view)
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

static void build_table_header(GtkWidget *table)
{
  for (int i = 0; i < SENSOR_HEADER_COLUMN_COUNT; i++) {
    GtkWidget *header_col = create_table_header(headers[i], widths[i]);
    gtk_widget_set_hexpand(header_col, TRUE);
    gtk_grid_attach(GTK_GRID(table), header_col, i, 0, 1, 1);
  }
}

static void build_table_row(sensor_view_t *view, sensor_t sensor, int row)
{
  const char *css_class = (row % 2 == 0) ? "table-row-even" : "table-row-odd";
  
  char value[VALUE_MAX];
  snprintf(value, VALUE_MAX, "%0.2f %s", sensor.value, sensor.unit);

  char datetime[DATETIME_MAX];
  format_timestamp_to_datetime(sensor.read_at, datetime);
  GtkWidget *columns[] = {
    create_table_cell(sensor.code, CELL_CODE_WIDTH),
    create_table_cell(sensor.type, CELL_TYPE_WIDTH),
    create_table_cell(value, CELL_VALUE_WIDTH),
    build_status_cell(sensor.status),
    create_table_cell(datetime, CELL_READ_AT_WIDTH)
  };

  for (int i = 0; i < SENSOR_TABLE_COLUMN_COUNT; i++) {
    
    gtk_widget_add_css_class(columns[i], css_class);
    gtk_widget_set_hexpand(columns[i], TRUE);

    gtk_grid_attach(view->table, columns[i], i, row, 1, 1);
  }
}

static GtkWidget *build_status_cell(sensor_status_t status)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "table-cell");
  gtk_widget_add_css_class(box, "status-cell");

  GtkWidget *border, *label, *image;

  border = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_add_css_class(border, "status-badge");

  label = gtk_label_new(sensor_status_to_string(status));
  gtk_widget_add_css_class(label, "status-label");

  switch (status) 
  {
    case SENSOR_OK:
      image = gtk_image_new_from_file("assets/status-operational.svg");
      gtk_widget_add_css_class(border, "status-operational");
      break;
    case SENSOR_WARNING:
      image = gtk_image_new_from_file("assets/status-maintenance.svg");
      gtk_widget_add_css_class(border, "status-maintenance");
      break;
    case SENSOR_CRITICAL:
      image = gtk_image_new_from_file("assets/status-failed.svg");
      gtk_widget_add_css_class(border, "status-failed");
      break;
    case SENSOR_NET_FAILURE:
      image = gtk_image_new_from_file("assets/status-disabled.svg");
      gtk_widget_add_css_class(border, "status-disabled");
      break;
  }

  gtk_image_set_pixel_size(GTK_IMAGE(image), 6);

  gtk_box_append(GTK_BOX(box), border);
  gtk_box_append(GTK_BOX(border), image);
  gtk_box_append(GTK_BOX(border), label);

  return box;
}

static void sensor_view_apply_filters(sensor_view_t *view)
{
  int position_status = gtk_drop_down_get_selected(view->status_filter);

  sensor_controller_set_filters(view->controller, position_status);
}

static void on_import_sensors_finish(GObject *self, GAsyncResult *res, gpointer data)
{
  sensor_view_t *view = (sensor_view_t *) data;

  GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(self), res, NULL);
  if (file == NULL) return;

  char *filepath = g_file_get_path(file);

  sensor_controller_request_file_import(view->controller, filepath);

  sensor_view_set_actions_enabled(view, false);

  free(filepath);
  g_object_unref(file);
}

static void on_import_sensors_clicked(GtkButton *button, gpointer data)
{
  (void) button; // unused

  sensor_view_t *view = (sensor_view_t *) data;

  GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(view->container)));

  GtkFileDialog *dialog = gtk_file_dialog_new();
  gtk_file_dialog_set_modal(dialog, TRUE);

  gtk_file_dialog_open(dialog, window, NULL, on_import_sensors_finish, view);

  g_object_unref(dialog);
}

static void on_fetch_api_clicked(GtkButton *button, gpointer data)
{
  (void) button; // unused

  sensor_view_t *view = (sensor_view_t *) data;

  sensor_controller_request_api_import(view->controller);

  sensor_view_set_actions_enabled(view, false);
}

static void on_search_entry_changed(GtkSearchEntry *search, gpointer data)
{
  sensor_view_t *view = (sensor_view_t *)data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(search));

  sensor_controller_set_search(view->controller, text);
}

static void on_date_entry_changed(GtkEntry *entry, gpointer data)
{
  sensor_view_t *view = (sensor_view_t *)data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));

  if (!sensor_controller_validate_date(text)) return;

  sensor_controller_set_date(view->controller, text);
}

static void on_filter_dropdown_changed(GObject *self, GParamSpec *pspec, gpointer data)
{
  (void)self; // unused parameter
  (void)pspec; // unused parameter

  sensor_view_t *view = (sensor_view_t *) data;

  sensor_view_apply_filters(view);
}

static void on_previous_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  sensor_view_t *view = (sensor_view_t *)data;

  pagination_previous(&view->controller->pagination);

  sensor_view_update(view);
}

static void on_next_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter 

  sensor_view_t *view = (sensor_view_t *)data;

  pagination_next(&view->controller->pagination);

  sensor_view_update(view);
}

static void on_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter 
  
  sensor_view_t *view = (sensor_view_t *)data;

  int page_number = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "page-number"));

  pagination_set_page_number(&view->controller->pagination, page_number);

  sensor_view_update(view);
}
