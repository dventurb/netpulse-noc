#include "ui_incident.h"

#include "utils.h"
#include "ui_widgets.h"
#include "macros.h"
#include "ui_macros.h"
#include "controller.h"

// TODO: look for better way
static const char* const incident_priority[] = { "Low", "Medium", "High", "Critical", NULL };
static const char* const incident_type[] = { "Equipment", "Sensor", NULL };
static const char* const filter_status[] = { "All", "Pending", "In Progress", "Concluded", NULL };
static const char* const filter_priority[] = { "All", "Low", "Medium", "High", "Critical", NULL };
static const char* const headers[] = { "ID", "QUEUE", "SOURCE", "TYPE", "DESCRIPTION", "PRIORITY", "STATUS", "TECHNICIAN", "CREATED", "CONCLUDED"};
static const int widths[] = { CELL_ID_WIDTH, CELL_QUEUE_WIDTH, CELL_SOURCE_WIDTH, CELL_TYPE_WIDTH, CELL_DESCRIPTION_WIDTH, CELL_PRIORITY_WIDTH, CELL_STATUS_WIDTH, CELL_TECHNICIAN_WIDTH, CELL_CREATED_WIDTH, CELL_CONCLUDED_WIDTH };

static const char* DATA_UI_INCIDENT = "ui-incident";
static const char* DATA_INCIDENT = "incident-id";
static const char* DATA_PRIORITY_FILTER = "incident-priority-filter";
static const char* DATA_STATUS_FILTER = "incident-status-filter";
static const char* DATA_RESOLVE_BUTTON = "incident-resolve-button";
static const char* DATA_STATS_CARDS = "incident-stats-cards";
static const char* DATA_PAGINATION_BAR = "incident-pagination-bar";

static const int INCIDENT_HEADER_COLUMN_COUNT = 10;
static const int INCIDENT_TABLE_COLUMN_COUNT = 11;

static GtkWidget *create_side_bar(application_t *application);
static GtkWidget *create_content(ui_incident_t *ui_incident);
static GtkWidget *create_incident_header(ui_incident_t *ui_incident);
static GtkWidget *create_incident_stats_cards(ui_incident_t *ui_incident);
static GtkWidget *create_incident_filters(ui_incident_t *ui_incident);
static GtkWidget *create_incident_table(ui_incident_t *ui_incident);
static GtkWidget *create_pagination_bar(ui_incident_t *ui_incident);
static void create_incident_table_header(GtkWidget *grid);
static void create_incident_table_row(GtkWidget *grid, incident_t incident, int row);

static GtkWidget *create_incident_priority_cell(incident_priority_t priority);
static GtkWidget *create_incident_status_cell(incident_status_t status);

static GtkWidget *create_incident_form(incident_queue_t *incidents_pending);

static void ui_incident_apply_filters(ui_incident_t *ui_incident);

// Callbacks
static void on_create_incident_clicked(GtkButton *button, gpointer data);
static void on_create_incident_confirmed(GtkButton *button, gpointer data);
static void on_process_next_incident_clicked(GtkButton *button, gpointer data);
static void on_resolve_incident_clicked(GtkButton *button, gpointer data);
static void on_incident_search_changed(GtkSearchEntry *search, gpointer data);
static void on_incident_filter_changed(GObject *self, GParamSpec *pspec, gpointer data);
static void on_incident_check_button_toggled(GtkCheckButton *button, gpointer data);
static void on_previous_page_clicked(GtkButton *button, gpointer data);
static void on_next_page_clicked(GtkButton *button, gpointer data);
static void on_page_clicked(GtkButton *button, gpointer data);


GtkWidget *create_page_incident(ui_t *ui)
{
  ui_incident_t *ui_incident = malloc(sizeof(ui_incident_t));
  if (ui_incident == NULL) return NULL;

  ui_incident->application = ui->application;
  ui_incident->window = ui->window;

  ui_incident->controller.selected_count = 0;
  ui_incident->controller.selected_node = NULL;
  ui_incident->controller.pagination.page = 0;
  ui_incident->controller.pagination.page_size = 6;

  int count = incident_get_count(&ui_incident->application->incidents_pending, &ui_incident->application->incidents_history);
  
  ui_incident->controller.pagination.total = pagination_total_pages(ui_incident->controller.pagination, count);

  ui_incident->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  GtkWidget *side_bar = create_side_bar(ui_incident->application);
  GtkWidget *content = create_content(ui_incident);

  gtk_box_append(GTK_BOX(ui_incident->container), side_bar);
  gtk_box_append(GTK_BOX(ui_incident->container), content);

  g_object_set_data_full(G_OBJECT(ui_incident->container), "ui_incident", ui_incident, free); // ownership + free

  return ui_incident->container;
}

void ui_incident_refresh(ui_incident_t *ui_incident)
{
  incident_controller_refresh_page(ui_incident);
}

void ui_incident_update_header(ui_incident_t *ui_incident)
{
  GtkWidget *resolve_incident_button = g_object_get_data(G_OBJECT(ui_incident->container), DATA_RESOLVE_BUTTON);

  if (ui_incident->controller.selected_node)
    gtk_widget_set_sensitive(resolve_incident_button, ui_incident->controller.selected_node->data.status == INCIDENT_IN_PROGRESS);
  else 
    gtk_widget_set_sensitive(resolve_incident_button, FALSE);
}

void ui_incident_update_stats_cards(ui_incident_t *ui_incident)
{
  GtkWidget *box = g_object_get_data(G_OBJECT(ui_incident->container), DATA_STATS_CARDS);

  remove_all_children_from_widget(box);

  incident_queue_t *queue = &ui_incident->application->incidents_pending;
  incident_list_t *list = &ui_incident->application->incidents_history;

  int total = incident_get_count(queue, list);
  int pending = incident_queue_get_count(queue);
  int in_progress = incident_list_get_number_status(list, INCIDENT_IN_PROGRESS);
  int concluded = incident_list_get_number_status(list, INCIDENT_CONCLUDED);

  GtkWidget *total_card = create_stats_card("Total Incidents", total, "default-card");
  GtkWidget *pending_card = create_stats_card("Pending", pending, "pending-card");
  GtkWidget *in_progress_card = create_stats_card("In Progress", in_progress, "in-progress-card");
  GtkWidget *concluded_card = create_stats_card("Concluded", concluded, "concluded-card");

  gtk_box_append(GTK_BOX(box), total_card);
  gtk_box_append(GTK_BOX(box), pending_card);
  gtk_box_append(GTK_BOX(box), in_progress_card);
  gtk_box_append(GTK_BOX(box), concluded_card);
}

void ui_incident_update_table(ui_incident_t *ui_incident, incident_t *incidents, int count)
{
  remove_table_rows(ui_incident->table);

  if (incidents == NULL || count == 0) return;

  for (int i = 0; i < count; i++) 
  {
    create_incident_table_row(ui_incident->table, incidents[i], i + 1);
  }

  GtkWidget *old_pagination = g_object_get_data(G_OBJECT(ui_incident->container), DATA_PAGINATION_BAR);

  if (old_pagination != NULL)
  {
    GtkWidget *table_container = gtk_widget_get_parent(old_pagination);
    
    if (table_container != NULL)
    {
      gtk_box_remove(GTK_BOX(table_container), old_pagination);

      GtkWidget *new_pagination = create_pagination_bar(ui_incident);
      gtk_box_append(GTK_BOX(table_container), new_pagination);
    }
  }
}

// TODO
static GtkWidget *create_side_bar(application_t *application)
{
  (void)application; // unused parameter

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 280, -1);

  return box;
}

static GtkWidget *create_content(ui_incident_t *ui_incident)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_set_margin_start(box, 24);
  gtk_widget_set_margin_end(box, 24);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);
  //gtk_widget_set_size_request(box, 1160, -1);

  GtkWidget *header = create_incident_header(ui_incident);
  GtkWidget *stats_cards = create_incident_stats_cards(ui_incident);
  GtkWidget *filters = create_incident_filters(ui_incident);
  GtkWidget *table = create_incident_table(ui_incident);

  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), stats_cards);
  gtk_box_append(GTK_BOX(box), filters);
  gtk_box_append(GTK_BOX(box), table);

  return box;
}

static GtkWidget *create_incident_header(ui_incident_t *ui_incident)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

  GtkWidget *title = gtk_label_new("Incident Management");
  gtk_widget_add_css_class(title, "incident-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  GtkWidget *create_incident_button = create_secondary_button("Create Incident", "assets/icon-add.svg", "secondary-button");
  g_signal_connect(create_incident_button, "clicked", G_CALLBACK(on_create_incident_clicked), ui_incident);

  GtkWidget *process_next_button = create_secondary_button("Process Next", NULL, "process-button");
  g_signal_connect(process_next_button, "clicked", G_CALLBACK(on_process_next_incident_clicked), ui_incident);

  GtkWidget *resolve_incident_button = create_secondary_button("Resolve Incident", NULL, "resolve-button");
  gtk_widget_set_sensitive(resolve_incident_button, FALSE);
  g_object_set_data(G_OBJECT(ui_incident->container), DATA_RESOLVE_BUTTON, resolve_incident_button);
  g_signal_connect(resolve_incident_button, "clicked", G_CALLBACK(on_resolve_incident_clicked), ui_incident);

  gtk_box_append(GTK_BOX(box), title);
  gtk_box_append(GTK_BOX(box), process_next_button);
  gtk_box_append(GTK_BOX(box), resolve_incident_button);
  gtk_box_append(GTK_BOX(box), create_incident_button);

  return box;
}

static GtkWidget *create_incident_stats_cards(ui_incident_t *ui_incident)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(box, TRUE);
  g_object_set_data(G_OBJECT(ui_incident->container), DATA_STATS_CARDS, box);

  incident_queue_t *queue = &ui_incident->application->incidents_pending;
  incident_list_t *list = &ui_incident->application->incidents_history;

  int total = incident_get_count(queue, list);
  int pending = incident_queue_get_count(queue);
  int in_progress = incident_list_get_number_status(list, INCIDENT_IN_PROGRESS);
  int concluded = incident_list_get_number_status(list, INCIDENT_CONCLUDED);

  GtkWidget *total_card = create_stats_card("Total Incidents", total, "default-card");
  GtkWidget *pending_card = create_stats_card("Pending", pending, "pending-card");
  GtkWidget *in_progress_card = create_stats_card("In Progress", in_progress, "in-progress-card");
  GtkWidget *concluded_card = create_stats_card("Concluded", concluded, "concluded-card");

  gtk_box_append(GTK_BOX(box), total_card);
  gtk_box_append(GTK_BOX(box), pending_card);
  gtk_box_append(GTK_BOX(box), in_progress_card);
  gtk_box_append(GTK_BOX(box), concluded_card);

  return box;
}

static GtkWidget *create_incident_filters(ui_incident_t *ui_incident)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
  gtk_widget_set_hexpand(box, TRUE);

  GtkWidget *search = gtk_search_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(search), STRING_MAX - 1);
  gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search), "Search by ID, device or technician...");
  gtk_widget_add_css_class(search, "incident-search");
  gtk_widget_set_hexpand(search, TRUE);
  g_signal_connect(search, "search-changed", G_CALLBACK(on_incident_search_changed), ui_incident);

  GtkWidget *dropdown_priority = gtk_drop_down_new_from_strings(filter_priority);
  gtk_widget_add_css_class(dropdown_priority, "incident-filter");
  g_signal_connect(dropdown_priority, "notify::selected", G_CALLBACK(on_incident_filter_changed), ui_incident);
  g_object_set_data(G_OBJECT(ui_incident->container), DATA_PRIORITY_FILTER, dropdown_priority);

  GtkWidget *dropdown_status = gtk_drop_down_new_from_strings(filter_status);
  gtk_widget_add_css_class(dropdown_status, "incident-filter");
  g_signal_connect(dropdown_status, "notify::selected", G_CALLBACK(on_incident_filter_changed), ui_incident);
  g_object_set_data(G_OBJECT(ui_incident->container), DATA_STATUS_FILTER, dropdown_status);

  gtk_box_append(GTK_BOX(box), search);
  gtk_box_append(GTK_BOX(box), dropdown_priority);
  gtk_box_append(GTK_BOX(box), dropdown_status);

  return box;
}

static GtkWidget *create_incident_table(ui_incident_t *ui_incident)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_add_css_class(box, "incident");

  GtkWidget *scrolled_window = gtk_scrolled_window_new();
  gtk_widget_set_size_request(scrolled_window, -1, 456);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
  gtk_widget_add_css_class(scrolled_window, "table-scroll");

  ui_incident->table = gtk_grid_new();
  gtk_widget_set_hexpand(ui_incident->table, FALSE);
  gtk_widget_set_halign(ui_incident->table, GTK_ALIGN_FILL);
  gtk_widget_add_css_class(ui_incident->table, "table");

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), ui_incident->table);

  g_object_set_data(G_OBJECT(ui_incident->table), DATA_UI_INCIDENT, ui_incident);

  create_incident_table_header(ui_incident->table);

  GtkWidget *pagination_bar = create_pagination_bar(ui_incident);

  gtk_box_append(GTK_BOX(box), scrolled_window);
  gtk_box_append(GTK_BOX(box), pagination_bar);

  incident_controller_refresh_page(ui_incident);

  return box;
}

static GtkWidget *create_pagination_bar(ui_incident_t *ui_incident)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  gtk_widget_set_size_request(box, -1, 64);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_set_halign(box, GTK_ALIGN_END);
  g_object_set_data(G_OBJECT(ui_incident->container), DATA_PAGINATION_BAR, box);

  GtkWidget *previous_button = create_secondary_button(NULL, "assets/left-arrow.svg", "arrow-page");
  gtk_widget_set_margin_top(previous_button, 16);
  gtk_widget_set_margin_bottom(previous_button, 16);
  gtk_widget_set_size_request(previous_button, 32, 32);
  g_signal_connect(previous_button, "clicked", G_CALLBACK(on_previous_page_clicked), ui_incident);
  gtk_box_append(GTK_BOX(box), previous_button);

  int start, end;

  pagination_controller_get_range(ui_incident->controller.pagination, &start, &end);

  for (int i = start; i <= end; i++) 
  {
    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%d", i + 1);

    GtkWidget *button = create_pagination_button(ui_incident->controller.pagination, buffer, i);
    g_signal_connect(button, "clicked", G_CALLBACK(on_page_clicked), ui_incident);

    gtk_box_append(GTK_BOX(box), button);
  }

  GtkWidget *next_button = create_secondary_button(NULL, "assets/right-arrow.svg", "arrow-page");
  gtk_widget_set_margin_top(next_button, 16);
  gtk_widget_set_margin_bottom(next_button, 16);
  gtk_widget_set_margin_end(next_button, 24);
  g_signal_connect(next_button, "clicked", G_CALLBACK(on_next_page_clicked), ui_incident);
  gtk_widget_set_size_request(next_button, 32, 32);
 
  gtk_box_append(GTK_BOX(box), next_button);

  return box;
}

static void create_incident_table_header(GtkWidget *grid)
{
  GtkWidget *select_all_button = gtk_check_button_new();
  gtk_widget_add_css_class(select_all_button, "table-header-checkbox");
  gtk_widget_set_size_request(select_all_button, 60, -1);
  gtk_grid_attach(GTK_GRID(grid), select_all_button, 0, 0, 1, 1);

  for (int i = 0; i < INCIDENT_HEADER_COLUMN_COUNT; i++) 
  {
    GtkWidget *label = create_table_header(headers[i], widths[i]);
    gtk_grid_attach(GTK_GRID(grid), label, i + 1, 0, 1, 1);
  }
}

static void create_incident_table_row(GtkWidget *grid, incident_t incident, int row)
{
  ui_incident_t *ui_incident = g_object_get_data(G_OBJECT(grid), DATA_UI_INCIDENT);

  const char *css_class = (row % 2 == 0) ? "table-row-even" : "table-row-odd";

  GtkWidget *check_button = create_table_checkbox();
  g_object_set_data(G_OBJECT(check_button), DATA_INCIDENT, GINT_TO_POINTER(incident.number));
  g_signal_connect(check_button, "toggled", G_CALLBACK(on_incident_check_button_toggled), ui_incident);

  char number[ID_MAX];
  incident_format_id(incident.number, number);

  char position_buffer[12];
  //int position = incident_queue_get_position(queue, node);
  int position = -1;
  incident_format_position(position, position_buffer);

  char created[DATETIME_MAX];
  get_datetime(incident.created_at, created);

  char concluded[DATETIME_MAX];
  get_datetime(incident.concluded_at, concluded);

  GtkWidget *columns[] = {
    check_button,
    create_table_cell(number, CELL_ID_WIDTH),
    create_table_cell(position_buffer, CELL_QUEUE_WIDTH),
    create_table_cell(incident.source_id, CELL_SOURCE_WIDTH),
    create_table_cell(incident.type, CELL_TYPE_WIDTH),
    create_table_cell(incident.description, CELL_DESCRIPTION_WIDTH),
    create_incident_priority_cell(incident.priority),
    create_incident_status_cell(incident.status),
    create_table_cell(incident.technician_name, CELL_TECHNICIAN_WIDTH),
    create_table_cell(created, CELL_CREATED_WIDTH),
    create_table_cell(concluded, CELL_CONCLUDED_WIDTH)
  };

  for (int i = 0; i < INCIDENT_TABLE_COLUMN_COUNT; i++) 
  {
    if (i == 1) gtk_widget_add_css_class(columns[i], "table-bold-cell");

    gtk_widget_add_css_class(columns[i], css_class);

    gtk_grid_attach(GTK_GRID(grid), columns[i], i, row, 1, 1);
  }
}

static GtkWidget *create_incident_form(incident_queue_t *incidents_pending)
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

  GtkWidget *entry_id = create_text_field(grid, "Incident ID", NULL, 0, 0);
  gtk_widget_add_css_class(entry_id, "form-entry-disabled");
  gtk_editable_set_editable(GTK_EDITABLE(entry_id), FALSE);
  g_object_set_data(G_OBJECT(grid), "entry-number", entry_id);

  char id[ID_MAX];
  incident_format_id(incidents_pending->next_number, id);
  gtk_editable_set_text(GTK_EDITABLE(entry_id), id);

  GtkWidget *entry_technician = create_text_field(grid, "Technician Name", NULL, 0, 1);
  gtk_entry_set_max_length(GTK_ENTRY(entry_technician), STRING_MAX - 1);
  g_object_set_data(G_OBJECT(grid), "entry-technician", entry_technician);

  GtkWidget *dropdown_source_type = create_dropdown_field(grid, "Type", incident_type, 1, 0);
  gtk_widget_set_sensitive(dropdown_source_type, FALSE);
  g_object_set_data(G_OBJECT(grid), "dropdown-type", dropdown_source_type);

  GtkWidget *entry_source_id = create_text_field(grid, "Source ID", "EQ-001", 1, 1);
  gtk_entry_set_max_length(GTK_ENTRY(entry_source_id), STRING_MAX - 1);
  g_object_set_data(G_OBJECT(grid), "entry-source-id", entry_source_id);

  GtkWidget *entry_type = create_text_field(grid, "Type", "Offline", 2, 0);
  gtk_entry_set_max_length(GTK_ENTRY(entry_type), STRING_MAX - 1);
  g_object_set_data(G_OBJECT(grid), "entry-type", entry_type);

  GtkWidget *dropdown_priority = create_dropdown_field(grid, "Priority", incident_priority, 2, 1);
  g_object_set_data(G_OBJECT(grid), "dropdown-priority", dropdown_priority);

  GtkWidget *entry_description = create_text_field(grid, "Description", "Device did not respond to ping. Possible network or device failure.", 3, 0);
  gtk_entry_set_max_length(GTK_ENTRY(entry_description), DESCRIPTION_MAX - 1);
  g_object_set_data(G_OBJECT(grid), "entry-description", entry_description);

  return grid;
}

static GtkWidget *create_incident_priority_cell(incident_priority_t priority)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "table-cell");
  gtk_widget_add_css_class(box, "priority-cell");

  GtkWidget *border, *label;

  border = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_add_css_class(border, "priority-badge");

  label = gtk_label_new(incident_priority_to_string(priority));
  gtk_widget_add_css_class(label, "priority-label");

  switch (priority) 
  {
    case PRIORITY_LOW:
      gtk_widget_add_css_class(border, "priority-low");
      break;
    case PRIORITY_MEDIUM:
      gtk_widget_add_css_class(border, "priority-medium");
      break;
    case PRIORITY_HIGH:
      gtk_widget_add_css_class(border, "priority-high");
      break;
    case PRIORITY_CRITICAL:
      gtk_widget_add_css_class(border, "priority-critical");
      break;
  }

  gtk_box_append(GTK_BOX(box), border);
  gtk_box_append(GTK_BOX(border), label);

  return box;
}

static GtkWidget *create_incident_status_cell(incident_status_t status)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "table-cell");
  gtk_widget_add_css_class(box, "status-cell");

  GtkWidget *border, *label, *image;

  border = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_add_css_class(border, "status-badge");

  label = gtk_label_new(incident_status_to_string(status));
  gtk_widget_add_css_class(label, "status-label");

  switch (status) 
  {
    case INCIDENT_PENDING:
      image = gtk_image_new_from_file("assets/status-maintenance.svg");
      gtk_widget_add_css_class(border, "status-pending");
      break;
    case INCIDENT_IN_PROGRESS:
      image = gtk_image_new_from_file("assets/status-in-progress.svg");
      gtk_widget_add_css_class(border, "status-in-progress");
      break;
    case INCIDENT_CONCLUDED:
      image = gtk_image_new_from_file("assets/status-operational.svg");
      gtk_widget_add_css_class(border, "status-concluded");
      break;
  }

  gtk_image_set_pixel_size(GTK_IMAGE(image), 6);

  gtk_box_append(GTK_BOX(box), border);
  gtk_box_append(GTK_BOX(border), image);
  gtk_box_append(GTK_BOX(border), label);

  return box;
}

static void ui_incident_apply_filters(ui_incident_t *ui_incident)
{
  GtkWidget *dropdown_status = g_object_get_data(G_OBJECT(ui_incident->container), DATA_STATUS_FILTER);
  GtkWidget *dropdown_priority = g_object_get_data(G_OBJECT(ui_incident->container), DATA_PRIORITY_FILTER);

  int position_status = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_status));
  int position_priority = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_priority));

  incident_controller_apply_filters(ui_incident, position_status, position_priority);
}

static void on_create_incident_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  ui_incident_t *ui_incident = (ui_incident_t *) data;

  incident_form_t *incident_form = malloc(sizeof(incident_form_t));
  if (incident_form == NULL) return;

  incident_form->application = ui_incident->application;
  incident_form->form = create_incident_form(&ui_incident->application->incidents_pending);

  dialog_config_t dialog_config = 
  {
      .window = ui_incident->window,
      .form = incident_form->form,
      .title = "Create Incident",
      .dialog_action = 
      {
        .label = "Create Incident",
        .icon = "assets/icon-add.svg",
        .css = "dialog-footer-add-button",
        .callback = G_CALLBACK(on_create_incident_confirmed),
        .data = incident_form
      }
  };

  incident_form->table = ui_incident->table;
  incident_form->dialog = create_dialog_window(dialog_config);

  g_object_set_data_full(G_OBJECT(incident_form->dialog), "incident-form", incident_form, free); // ownership + free

  gtk_window_present(GTK_WINDOW(incident_form->dialog));
}

static void on_create_incident_confirmed(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  incident_form_t *incident_form = (incident_form_t *) data;

  incident_t new;

  form_field_t fields[] = {
    { "entry-technician", new.technician_name },
    { "entry-source-id", new.source_id },
    { "entry-type", new.type },
    { "entry-description", new.description},
  };

  for (int i = 0; i < 4; i++)
  {
    GtkWidget *entry = g_object_get_data(G_OBJECT(incident_form->form), fields[i].key);
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    if (i == 4) 
      snprintf(fields[i].dest, DESCRIPTION_MAX, "%s", text);
    else  
      snprintf(fields[i].dest, STRING_MAX, "%s", text);
  }

  GtkWidget *dropdown_source_type = g_object_get_data(G_OBJECT(incident_form->form), "dropdown-type");
  new.source_type = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_source_type));

  GtkWidget *dropdown_priority = g_object_get_data(G_OBJECT(incident_form->form), "dropdown-priority");
  new.priority = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown_priority));

  ui_incident_t *ui_incident = g_object_get_data(G_OBJECT(incident_form->table), DATA_UI_INCIDENT);

  incident_controller_add(ui_incident, new);

  gtk_window_destroy(GTK_WINDOW(incident_form->dialog));
}

static void on_process_next_incident_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter

  ui_incident_t *ui_incident = (ui_incident_t *) data;
  incident_controller_process(ui_incident);
}

static void on_resolve_incident_clicked(GtkButton *button, gpointer data)
{
  (void) button; // unused parameter

  ui_incident_t *ui_incident = (ui_incident_t *)data;
  incident_controller_resolve(ui_incident);
}

static void on_incident_search_changed(GtkSearchEntry *search, gpointer data)
{
  ui_incident_t *ui_incident = (ui_incident_t *)data;

  const char *text = gtk_editable_get_text(GTK_EDITABLE(search));

  incident_controller_search(ui_incident, text);
}

static void on_incident_filter_changed(GObject *self, GParamSpec *pspec, gpointer data)
{
  (void)self; // unused parameter
  (void)pspec; // unused parameter

  ui_incident_t *ui_incident = (ui_incident_t *) data;

  ui_incident_apply_filters(ui_incident);
}

static void on_incident_check_button_toggled(GtkCheckButton *button, gpointer data)
{
  ui_incident_t *ui_incident = (ui_incident_t *) data;

  int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), DATA_INCIDENT));
  bool is_active = gtk_check_button_get_active(button);

  incident_controller_handle_toggled(ui_incident, id, is_active);
}

static void on_previous_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter
  
  ui_incident_t *ui_incident = (ui_incident_t *)data;

  pagination_controller_previous(&ui_incident->controller.pagination);

  incident_controller_update_table(ui_incident);
}

static void on_next_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter 

  ui_incident_t *ui_incident = (ui_incident_t *)data;

  pagination_controller_next(&ui_incident->controller.pagination);

  incident_controller_update_table(ui_incident);
}

static void on_page_clicked(GtkButton *button, gpointer data)
{
  (void)button; // unused parameter 
  
  ui_incident_t *ui_incident = (ui_incident_t *)data;

  int page_number = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "page-number"));

  pagination_controller_page_number(&ui_incident->controller.pagination, page_number);
}
