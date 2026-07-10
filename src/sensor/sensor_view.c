#include "sensor_view.h"

#include "utils.h"
#include "macros.h"

#include "action_button.h"
#include "stats_card.h"
#include "table_cell.h"
#include "status_badge.h"
#include "widget_utils.h"

static const char* const filter_status[] = { "All", "OK", "Warning", "Critical", "Network Failure", NULL };

static const int SENSOR_TABLE_COLUMN_COUNT = 5;

static GtkWidget *build_sidebar(sensor_view_t *view);
static GtkWidget *build_content(sensor_view_t *view);
static GtkWidget *build_header(sensor_view_t *view);
static GtkWidget *build_stats_cards(sensor_view_t *view);
static GtkWidget *build_filter_bar(sensor_view_t *view);

static GtkWidget *build_table(sensor_view_t *view);
static void build_table_row(sensor_view_t *view, sensor_t sensor);

static GtkWidget *build_status_cell(sensor_status_t status);

static void sensor_view_apply_filters(sensor_view_t *view);

// Callbacks
static void on_import_sensors_clicked(GtkButton *button, gpointer data);
static void on_fetch_api_clicked(GtkButton *button, gpointer data);

static void on_search_entry_changed(GtkSearchEntry *search, gpointer data);
static void on_date_entry_changed(GtkEntry *entry, gpointer data);
static void on_filter_dropdown_changed(GObject *self, GParamSpec *pspec, gpointer data);

static void on_page_clicked(void *data);


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

void sensor_view_destroy(sensor_view_t *view)
{
  table_destroy(&view->table);
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
  sensor_stats_t stats = {0};

  sensor_controller_get_stats(view->controller, &stats);

  stats_card_set_value(&view->total_card, stats.total);
  stats_card_set_value(&view->ok_card, stats.ok);
  stats_card_set_value(&view->warning_card, stats.warning_critical); // TODO
  stats_card_set_value(&view->failed_card, stats.failure);
}

void sensor_view_update_table(sensor_view_t *view, const sensor_t *sensors, int count)
{
  table_clear_rows(&view->table);

  if (sensors == NULL || count == 0) return;

  for (int i = 0; i < count; i++) 
    build_table_row(view, sensors[i]);

  table_refresh(&view->table);
}

void sensor_view_set_actions_enabled(sensor_view_t *view, bool is_active)
{
  gtk_widget_set_sensitive(GTK_WIDGET(view->import_button), is_active);
  gtk_widget_set_sensitive(GTK_WIDGET(view->fetch_button), is_active);
}

// TODO : sidebar with charts (Using Cairo and GtkChart lib)
static GtkWidget *build_sidebar(sensor_view_t *view)
{
  (void)view; // unused parameter

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 280, -1);

  return box;
}

static GtkWidget *build_content(sensor_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_hexpand(container, TRUE);
  gtk_widget_set_margin_start(container, 24);
  gtk_widget_set_margin_end(container, 24);
  gtk_widget_set_margin_top(container, 24);
  gtk_widget_set_margin_bottom(container, 24);
  //gtk_widget_set_size_request(container, 1160, -1);

  GtkWidget *header = build_header(view);
  GtkWidget *cards = build_stats_cards(view);
  GtkWidget *filters = build_filter_bar(view);
  GtkWidget *table = build_table(view);

  gtk_box_append(GTK_BOX(container), header);
  gtk_box_append(GTK_BOX(container), cards);
  gtk_box_append(GTK_BOX(container), filters);
  gtk_box_append(GTK_BOX(container), table);

  return container;
}

static GtkWidget *build_header(sensor_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

  GtkWidget *title = gtk_label_new("Sensors Monitoring");
  gtk_widget_add_css_class(title, "inventory-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  view->import_button = GTK_BUTTON(action_button_new("Import Sensors", "assets/icon-import.svg", "import-button"));
  g_signal_connect(GTK_WIDGET(view->import_button), "clicked", G_CALLBACK(on_import_sensors_clicked), view);

  view->fetch_button = GTK_BUTTON(action_button_new("Fetch API", "assets/icon-fetch.svg", "fetch-button"));
  g_signal_connect(GTK_WIDGET(view->fetch_button), "clicked", G_CALLBACK(on_fetch_api_clicked), view);

  gtk_box_append(GTK_BOX(box), title);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->import_button));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->fetch_button));

  return box;
}

static GtkWidget *build_stats_cards(sensor_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(container, TRUE);

  sensor_stats_t stats = {0};

  sensor_controller_get_stats(view->controller, &stats);

  view->total_card = stats_card_new("Total Sensors", stats.total, "default-card");
  view->ok_card = stats_card_new("OK", stats.ok, "operational-card");
  view->warning_card = stats_card_new("Warning + Critical", stats.warning_critical, "maintenance-card"); // TODO
  view->failed_card = stats_card_new("Network Failures", stats.failure, "failed-card");

  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->total_card.container));
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->ok_card.container));
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->warning_card.container));
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->failed_card.container));

  return container;
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
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  table_column_t columns[] = {
    { "CODE",    CELL_CODE_WIDTH },
    { "TYPE",    CELL_TYPE_WIDTH },
    { "VALUE",   CELL_VALUE_WIDTH },
    { "STATUS",  CELL_STATUS_WIDTH },
    { "READ AT", CELL_READ_AT_WIDTH }
  };

  view->table = table_new(columns, SENSOR_TABLE_COLUMN_COUNT, FALSE);
  table_set_pagination(&view->table, &view->controller->pagination, on_page_clicked, view);

  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->table.container));

  return container;
}

static void build_table_row(sensor_view_t *view, sensor_t sensor)
{
  table_row_t row = table_row_new(SENSOR_TABLE_COLUMN_COUNT);

  char value[VALUE_MAX];
  snprintf(value, VALUE_MAX, "%0.2f %s", sensor.value, sensor.unit);

  char datetime[DATETIME_MAX];
  format_timestamp_to_datetime(sensor.read_at, datetime);

  table_row_insert_cell(&row, table_cell_new(sensor.code, CELL_CODE_WIDTH, "table-cell"));
  table_row_insert_cell(&row, table_cell_new(sensor.type, CELL_TYPE_WIDTH, "table-cell"));
  table_row_insert_cell(&row, table_cell_new(value, CELL_VALUE_WIDTH, "table-cell"));
  table_row_insert_cell(&row, build_status_cell(sensor.status));
  table_row_insert_cell(&row, table_cell_new(datetime, CELL_READ_AT_WIDTH, "table-cell"));

  table_insert_row(&view->table, &row);
}

static GtkWidget *build_status_cell(sensor_status_t status)
{
  const char *icons[] = {
    "assets/status-operational.svg",
    "assets/status-maintenance.svg",
    "assets/status-failed.svg",
    "assets/status-disabled.svg"
  };

  const char *css[] = {
    "status-operational",
    "status-maintenance",
    "status-failed",
    "status-disabled"
  };
    
  return status_badge_new(sensor_status_to_string(status), icons[status], css[status]);
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

static void on_page_clicked(void *data)
{
  sensor_view_t *view = (sensor_view_t *)data;

  sensor_view_update(view);
}
