#include "incident_view.h"

#include "utils.h"
#include "macros.h"

#include "action_button.h"
#include "dialog.h"
#include "table_header.h"
#include "table_cell.h"
#include "table_checkbox.h"
#include "status_badge.h"
#include "priority_badge.h"
#include "widget_utils.h"

// TODO: look for better way
static const char* const incident_priority[] = { "Low", "Medium", "High", "Critical", NULL };
static const char* const incident_type[] = { "Equipment", "Sensor", NULL };
static const char* const filter_status[] = { "All", "Pending", "In Progress", "Concluded", NULL };
static const char* const filter_priority[] = { "All", "Low", "Medium", "High", "Critical", NULL };
static const char* const headers[] = { "ID", "QUEUE", "SOURCE", "TYPE", "DESCRIPTION", "PRIORITY", "STATUS", "TECHNICIAN", "CREATED", "CONCLUDED"};
static const int widths[] = { CELL_NUMBER_WIDTH, CELL_QUEUE_WIDTH, CELL_SOURCE_WIDTH, CELL_TYPE_WIDTH, CELL_DESCRIPTION_WIDTH, CELL_PRIORITY_WIDTH, CELL_STATUS_WIDTH, CELL_TECHNICIAN_WIDTH, CELL_CREATED_WIDTH, CELL_CONCLUDED_WIDTH };

static const char* DATA_INCIDENT = "INCIDENT-ID";

static const int INCIDENT_HEADER_COLUMN_COUNT = 10;
static const int INCIDENT_TABLE_COLUMN_COUNT = 11;

static GtkWidget *build_sidebar(incident_view_t *view);
static GtkWidget *build_content(incident_view_t *view);
static GtkWidget *build_header(incident_view_t *view);
static GtkWidget *build_stats_cards(incident_view_t *view);
static GtkWidget *build_filter_bar(incident_view_t *view);
static GtkWidget *build_table(incident_view_t *view);

static void build_table_header(GtkWidget *grid);
static void build_table_row(incident_view_t *view, incident_t incident, int row);
static GtkWidget *build_priority_cell(incident_priority_t priority);
static GtkWidget *build_status_cell(incident_status_t status);

static GtkWidget *build_form(incident_view_t *view);

static void incident_view_apply_filters(incident_view_t *view);

// Callbacks
static void on_create_incident_clicked(GtkButton *button, gpointer data);
static void on_process_incident_clicked(GtkButton *button, gpointer data);
static void on_resolve_incident_clicked(GtkButton *button, gpointer data);

static void on_create_form_submit(GtkButton *button, gpointer data);

static void on_search_entry_changed(GtkSearchEntry *search, gpointer data);
static void on_filter_dropdown_changed(GObject *self, GParamSpec *pspec, gpointer data);
static void on_row_selection_toggled(GtkCheckButton *button, gpointer data);
static void on_source_id_entry_changed(GtkEntry *entry, gpointer data);

static void on_page_clicked(void *data);


GtkBox *incident_view_create(incident_view_t *view, incident_controller_t *controller)
{
  view->controller = controller;

  view->container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

  GtkWidget *side_bar = build_sidebar(view);
  GtkWidget *content = build_content(view);

  gtk_box_append(view->container, side_bar);
  gtk_box_append(view->container, content);

  return view->container;
}

void incident_view_destroy(incident_view_t *view)
{
  pagination_bar_destroy(&view->pagination_bar);
}

void incident_view_refresh(incident_view_t *view)
{
  incident_controller_refresh_page(view->controller);
}

void incident_view_update_header(incident_view_t *view)
{
  if (view->controller->selected_node != NULL)
  {
    if (view->controller->selected_node->data.status == INCIDENT_IN_PROGRESS)
      gtk_widget_set_sensitive(GTK_WIDGET(view->resolve_button), TRUE);
  }

  else 
    gtk_widget_set_sensitive(GTK_WIDGET(view->resolve_button), FALSE);
}

void incident_view_update_stats_cards(incident_view_t *view)
{
  incident_stats_t stats = {0};

  incident_controller_get_stats(view->controller, &stats);

  stats_card_set_value(&view->total_card, stats.total);
  stats_card_set_value(&view->pending_card, stats.pending);
  stats_card_set_value(&view->in_progress_card, stats.in_progress);
  stats_card_set_value(&view->concluded_card, stats.concluded);
}

void incident_view_update_table(incident_view_t *view, incident_t *incidents, int count)
{
  table_remove_rows(GTK_WIDGET(view->table));

  if (incidents == NULL || count == 0) return;

  for (int i = 0; i < count; i++) 
    build_table_row(view, incidents[i], i + 1);

  pagination_bar_refresh(&view->pagination_bar);
}

// TODO
static GtkWidget *build_sidebar(incident_view_t *view)
{
  (void)view; // unused parameter

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 280, -1);

  return box;
}

static GtkWidget *build_content(incident_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_hexpand(container, TRUE);
  gtk_widget_set_margin_start(container, 24);
  gtk_widget_set_margin_end(container, 24);
  gtk_widget_set_margin_top(container, 24);
  gtk_widget_set_margin_bottom(container, 24);
  //gtk_widget_set_size_request(box, 1160, -1);

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

static GtkWidget *build_header(incident_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

  GtkWidget *title = gtk_label_new("Incident Management");
  gtk_widget_add_css_class(title, "incident-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  view->process_button = GTK_BUTTON(action_button_new("Process Next", NULL, "process-button"));
  g_signal_connect(GTK_WIDGET(view->process_button), "clicked", G_CALLBACK(on_process_incident_clicked), view);

  view->resolve_button = GTK_BUTTON(action_button_new("Resolve Incident", NULL, "resolve-button"));
  gtk_widget_set_sensitive(GTK_WIDGET(view->resolve_button), FALSE);
  g_signal_connect(GTK_WIDGET(view->resolve_button), "clicked", G_CALLBACK(on_resolve_incident_clicked), view);

  view->create_button = GTK_BUTTON(action_button_new("Create Incident", "assets/icon-add.svg", "secondary-button"));
  g_signal_connect(GTK_WIDGET(view->create_button), "clicked", G_CALLBACK(on_create_incident_clicked), view);

  gtk_box_append(GTK_BOX(box), title);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->process_button));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->resolve_button));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->create_button));

  return box;
}

static GtkWidget *build_stats_cards(incident_view_t *view)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(container, TRUE);

  incident_stats_t stats = {0};

  incident_controller_get_stats(view->controller, &stats);

  view->total_card = stats_card_new("Total Incidents", stats.total, "default-card");
  view->pending_card = stats_card_new("Pending", stats.pending, "pending-card");
  view->in_progress_card = stats_card_new("In Progress", stats.in_progress, "in-progress-card");
  view->concluded_card = stats_card_new("Concluded", stats.concluded, "concluded-card");

  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->total_card.container));
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->pending_card.container));
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->in_progress_card.container));
  gtk_box_append(GTK_BOX(container), GTK_WIDGET(view->concluded_card.container));

  return container;
}

static GtkWidget *build_filter_bar(incident_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(box, TRUE);

  GtkWidget *search = gtk_search_entry_new();
  gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search), "Search by ID, equipment or sensor...");
  gtk_widget_add_css_class(search, "incident-search");
  gtk_widget_set_hexpand(search, TRUE);
  g_signal_connect(search, "search-changed", G_CALLBACK(on_search_entry_changed), view);

  view->priority_filter = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(filter_priority));
  gtk_widget_add_css_class(GTK_WIDGET(view->priority_filter), "incident-filter");
  g_signal_connect(GTK_WIDGET(view->priority_filter), "notify::selected", G_CALLBACK(on_filter_dropdown_changed), view);

  view->status_filter = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(filter_status));
  gtk_widget_add_css_class(GTK_WIDGET(view->status_filter), "incident-filter");
  g_signal_connect(GTK_WIDGET(view->status_filter), "notify::selected", G_CALLBACK(on_filter_dropdown_changed), view);

  gtk_box_append(GTK_BOX(box), search);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->priority_filter));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->status_filter));

  return box;
}

static GtkWidget *build_table(incident_view_t *view)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_add_css_class(box, "incident");

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

  view->pagination_bar = pagination_bar_new(&view->controller->pagination, on_page_clicked, view);
  pagination_bar_refresh(&view->pagination_bar);
  pagination_bar_setup_callbacks(&view->pagination_bar);

  gtk_box_append(GTK_BOX(box), scrolled_window);
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(view->pagination_bar.container));

  incident_controller_refresh_page(view->controller);

  return box;
}

static void build_table_header(GtkWidget *table)
{
  GtkWidget *select_all_button = gtk_check_button_new();
  gtk_widget_add_css_class(select_all_button, "table-header-checkbox");
  gtk_widget_set_size_request(select_all_button, 60, -1);
  gtk_grid_attach(GTK_GRID(table), select_all_button, 0, 0, 1, 1);

  for (int i = 0; i < INCIDENT_HEADER_COLUMN_COUNT; i++) 
  {
    GtkWidget *label = table_header_new(headers[i], widths[i]);
    gtk_grid_attach(GTK_GRID(table), label, i + 1, 0, 1, 1);
  }
}

static void build_table_row(incident_view_t *view, incident_t incident, int row)
{
  const char *css_class = (row % 2 == 0) ? "table-row-even" : "table-row-odd";

  GtkWidget *check_button = table_checkbox_new();
  g_object_set_data(G_OBJECT(check_button), DATA_INCIDENT, GINT_TO_POINTER(incident.number));
  g_signal_connect(check_button, "toggled", G_CALLBACK(on_row_selection_toggled), view);

  char number[ID_MAX];
  incident_format_id(incident.number, number);

  char position_buffer[12];
  int position = incident_controller_get_position(view->controller, incident, row);
  incident_format_position(position, position_buffer);

  char created[DATETIME_MAX];
  format_timestamp_to_datetime(incident.created_at, created);

  char concluded[DATETIME_MAX];
  format_timestamp_to_datetime(incident.concluded_at, concluded);

  GtkWidget *columns[] = {
    check_button,
    table_cell_new(number, CELL_ID_WIDTH),
    table_cell_new(position_buffer, CELL_QUEUE_WIDTH),
    table_cell_new(incident.source_id, CELL_SOURCE_WIDTH),
    table_cell_new(incident.type, CELL_TYPE_WIDTH),
    table_cell_new(incident.description, CELL_DESCRIPTION_WIDTH),
    build_priority_cell(incident.priority),
    build_status_cell(incident.status),
    table_cell_new(incident.technician_name, CELL_TECHNICIAN_WIDTH),
    table_cell_new(created, CELL_CREATED_WIDTH),
    table_cell_new(concluded, CELL_CONCLUDED_WIDTH)
  };

  for (int i = 0; i < INCIDENT_TABLE_COLUMN_COUNT; i++) 
  {
    if (i == 1) gtk_widget_add_css_class(columns[i], "table-bold-cell");

    gtk_widget_add_css_class(columns[i], css_class);

    gtk_grid_attach(view->table, columns[i], i, row, 1, 1);
  }
}

static GtkWidget *build_form(incident_view_t *view)
{
  GtkWidget *grid = gtk_grid_new();
  gtk_widget_set_size_request(grid, 672, 500);
  gtk_widget_set_margin_start(grid, 24);
  gtk_widget_set_margin_end(grid, 24);
  gtk_widget_set_margin_top(grid, 24);
  gtk_widget_set_margin_bottom(grid, 40);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 24);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 24);
  gtk_widget_add_css_class(grid, "dialog-form");

  char number[NUMBER_MAX];
  incident_format_id(view->controller->data->incidents_pending.next_number, number);
  //gtk_editable_set_text(GTK_EDITABLE(view->form.number_field.entry), number);
  
  view->form.number_field = input_field_new("Incident Number", number, NULL);
  gtk_widget_add_css_class(GTK_WIDGET(view->form.number_field.container), "field-entry-disabled");
  gtk_editable_set_editable(GTK_EDITABLE(view->form.number_field.entry), FALSE);

  view->form.technician_field = input_field_new("Technician Name", NULL, NULL);
  gtk_widget_add_css_class(GTK_WIDGET(view->form.technician_field.container), "field-entry-disabled");
  gtk_editable_set_editable(GTK_EDITABLE(view->form.technician_field.entry), FALSE);
  gtk_editable_set_text(GTK_EDITABLE(view->form.technician_field.entry), view->controller->data->current_user->name);

  view->form.source_field = dropdown_field_new("Type", incident_type);
  gtk_widget_add_css_class(GTK_WIDGET(view->form.source_field.dropdown), "field-entry-disabled");
  gtk_widget_set_sensitive(GTK_WIDGET(view->form.source_field.dropdown), FALSE);

  view->form.source_id_field = input_field_new("Source ID", "EQ-001", NULL);
  gtk_entry_set_max_length(view->form.source_id_field.entry, CODE_MAX - 1);
  g_signal_connect(view->form.source_id_field.entry, "changed", G_CALLBACK(on_source_id_entry_changed), view);

  view->form.type_field = input_field_new("Type", "Offline", NULL);
  gtk_entry_set_max_length(view->form.type_field.entry, STRING_MAX - 1);

  view->form.priority_field = dropdown_field_new("Priority", incident_priority);

  view->form.description_field = input_field_new("Description", "Device did not respond to ping. Possible network or device failure.", NULL);
  gtk_entry_set_max_length(view->form.description_field.entry, DESCRIPTION_MAX - 1);

  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.number_field.container), 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.technician_field.container), 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.source_field.container), 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.source_id_field.container), 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.type_field.container), 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.priority_field.container), 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(view->form.description_field.container), 0, 3, 2, 1);

  return grid;
}

static GtkWidget *build_priority_cell(incident_priority_t priority)
{
  const char *css[] = {
    "priority-low",
    "priority-medium",
    "priority-high",
    "priority-critical"
  };

  return priority_badge_new(incident_priority_to_string(priority), css[priority]);
}

static GtkWidget *build_status_cell(incident_status_t status)
{
  const char *icons[] = {
    "assets/status-maintenance.svg",
    "assets/status-in-progress.svg",
    "assets/status-operational.svg"
  };

  const char *css[] = {
    "status-pending",
    "status-in-progress",
    "status-concluded",
  };

  return status_badge_new(incident_status_to_string(status), icons[status], css[status]);
}

static void incident_view_apply_filters(incident_view_t *view)
{
  int position_status = gtk_drop_down_get_selected(view->status_filter);
  int position_priority = gtk_drop_down_get_selected(view->priority_filter);

  incident_controller_apply_filters(view->controller, position_status, position_priority);
}

static void on_create_incident_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  incident_view_t *view = (incident_view_t *) data;

  view->form.layout = build_form(view);

  GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(view->container)));

  dialog_config_t config = 
  {
      .window = GTK_WIDGET(window),
      .form = view->form.layout,
      .title = "Create Incident",
      .action = 
      {
        .label = "Create Incident",
        .icon = "assets/icon-add.svg",
        .css = "dialog-footer-add-button",
        .callback = G_CALLBACK(on_create_form_submit),
        .data = view
      }
  };

  view->form.dialog = GTK_WINDOW(dialog_new(config));

  gtk_window_present(view->form.dialog);
}

static void on_create_form_submit(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  incident_view_t *view = (incident_view_t *) data;
  
  const char *technician_text = gtk_editable_get_text(GTK_EDITABLE(view->form.technician_field.entry));
  const char *source_id_text = gtk_editable_get_text(GTK_EDITABLE(view->form.source_id_field.entry));
  const char *type_text = gtk_editable_get_text(GTK_EDITABLE(view->form.type_field.entry));
  const char *description_text = gtk_editable_get_text(GTK_EDITABLE(view->form.description_field.entry));

  incident_t new = {0};

  snprintf(new.technician_name, STRING_MAX, "%s", technician_text);
  snprintf(new.type, STRING_MAX, "%s", type_text);
  snprintf(new.description, DESCRIPTION_MAX, "%s", description_text);

  convert_to_uppercase(source_id_text, new.source_id);
  new.source_type = gtk_drop_down_get_selected(view->form.source_field.dropdown);
  new.priority = gtk_drop_down_get_selected(view->form.priority_field.dropdown);

  gtk_widget_remove_css_class(GTK_WIDGET(view->form.source_id_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.type_field.container), "field-error");
  gtk_widget_remove_css_class(GTK_WIDGET(view->form.description_field.container), "field-error");

  incident_validation_t error = incident_controller_validate(view->controller, new);

  switch (error) 
  {
    case INCIDENT_INVALID_SOURCE_ID:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.source_id_field.container), "field-error");
      return;
    case INCIDENT_INVALID_TYPE:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.type_field.container), "field-error");
      return;
    case INCIDENT_INVALID_DESCRIPTION:
      gtk_widget_add_css_class(GTK_WIDGET(view->form.description_field.container), "field-error");
      return;
    case INCIDENT_VALID: break;
  }

  incident_controller_add(view->controller, new);

  gtk_window_destroy(view->form.dialog);
}

static void on_process_incident_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter

  incident_view_t *view = (incident_view_t *) data;
  incident_controller_process(view->controller);
}

static void on_resolve_incident_clicked(GtkButton *button, gpointer data)
{
  (void) button; // unused parameter

  incident_view_t *view = (incident_view_t *)data;
  incident_controller_resolve(view->controller);
}

static void on_search_entry_changed(GtkSearchEntry *search, gpointer data)
{
  incident_view_t *view = (incident_view_t *)data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(search));

  incident_controller_search(view->controller, text);
}

static void on_filter_dropdown_changed(GObject *self, GParamSpec *pspec, gpointer data)
{
  (void)self; // unused parameter
  (void)pspec; // unused parameter

  incident_view_t *view = (incident_view_t *) data;

  incident_view_apply_filters(view);
}

static void on_row_selection_toggled(GtkCheckButton *button, gpointer data)
{
  incident_view_t *view = (incident_view_t *) data;

  int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), DATA_INCIDENT));
  bool is_active = gtk_check_button_get_active(button);

  incident_controller_handle_toggled(view->controller, id, is_active);
}

static void on_source_id_entry_changed(GtkEntry *entry, gpointer data)
{
  incident_view_t *view = (incident_view_t *) data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
  if (strlen(text) <= 3) return;

  search_type_t source_type = detect_search_type(text);
  if (source_type == SEARCH_EQUIPMENT_ID)
    gtk_drop_down_set_selected(view->form.source_field.dropdown, 0);
  else if (source_type == SEARCH_SENSOR_CODE)
    gtk_drop_down_set_selected(view->form.source_field.dropdown, 1);
}

static void on_page_clicked(void *data)
{
  incident_view_t *view = (incident_view_t *)data;

  incident_controller_update_table(view->controller);
}
