#include "table.h"

#include "table_cell.h"

static void table_row_destroy(table_row_t *row);
static void table_append_header(table_t *table, table_column_t *columns, const int count, bool has_checkbox);


table_t table_new(table_column_t *columns, const int count, bool has_checkbox)
{
  table_t table = {0};

  table.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  gtk_widget_add_css_class(GTK_WIDGET(table.container), "inventory");

  table.scrolled = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
  gtk_widget_set_hexpand(GTK_WIDGET(table.scrolled), TRUE);
  gtk_widget_set_size_request(GTK_WIDGET(table.scrolled), -1, 456);
  gtk_widget_add_css_class(GTK_WIDGET(table.scrolled), "table-scroll");
  gtk_scrolled_window_set_policy(table.scrolled, GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);

  table.widget = GTK_GRID(gtk_grid_new());
  gtk_widget_set_hexpand(GTK_WIDGET(table.widget), FALSE);
  gtk_widget_set_halign(GTK_WIDGET(table.widget), GTK_ALIGN_FILL);
  gtk_widget_add_css_class(GTK_WIDGET(table.widget), "table");

  gtk_scrolled_window_set_child(table.scrolled, GTK_WIDGET(table.widget));

  table_append_header(&table, columns, count, has_checkbox);

  gtk_box_append(table.container, GTK_WIDGET(table.scrolled));

  return table;
}

void table_destroy(table_t *table)
{
  pagination_bar_destroy(&table->pagination_bar);
}

void table_set_pagination(table_t *table, pagination_t *pagination, pagination_bar_callback callback, void *data)
{
  table->pagination_bar = pagination_bar_new(pagination);
  pagination_bar_set_callback(&table->pagination_bar, callback, data);
  pagination_bar_refresh(&table->pagination_bar);

  gtk_box_append(table->container, GTK_WIDGET(table->pagination_bar.container));
}

void table_insert_row(table_t *table, table_row_t *row)
{
  const char *css = (table->current_row & 1) ? "table-row-odd" : "table-row-even";

  for (int i = 0; i < row->count; i++) 
  {
    gtk_widget_add_css_class(row->cells[i], css);
    gtk_widget_set_hexpand(row->cells[i], TRUE);

    gtk_grid_attach(table->widget, row->cells[i], i, table->current_row, 1, 1);
  }

  table->current_row++;

  table_row_destroy(row);
}

void table_refresh(table_t *table)
{
  pagination_bar_refresh(&table->pagination_bar);
}

void table_clear_rows(table_t *table)
{
  if (table->widget == NULL || table->current_row <= 1) return;

  table->current_row = 1;

  GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(table->widget));
  
  while (child != NULL)
  {
    GtkWidget *next = gtk_widget_get_next_sibling(child);
    
    int row;
    gtk_grid_query_child(table->widget, child, NULL, &row, NULL, NULL);
    
    if (row > 0)  gtk_grid_remove(table->widget, child); // Skip the headers

    child = next;
  }
}

table_row_t table_row_new(int count)
{
  table_row_t row = {0};

  row.cells = malloc(sizeof(GtkWidget *) * count);
  if (row.cells == NULL) return row;

  row.count = count;

  return row;
}

static void table_row_destroy(table_row_t *row)
{
  if (row->cells == NULL) return;

  free(row->cells);
  row->cells = NULL;
  row->count = 0;
  row->current = 0;
}

void table_row_insert_cell(table_row_t *row, GtkWidget *cell)
{
  if (row->cells == NULL || cell == NULL) return;
  if (row->current >= row->count) return;

  row->cells[row->current++] = cell;
}

static void table_append_header(table_t *table, table_column_t *columns, const int count, bool has_checkbox)
{
  const int size = count + (has_checkbox ? 1 : 0);

  table_row_t row = table_row_new(size);

  if (has_checkbox)
  {
    GtkWidget *select_button = gtk_check_button_new();
    gtk_widget_add_css_class(select_button, "table-header-checkbox");
    gtk_widget_set_size_request(select_button, 60, -1);

    table_row_insert_cell(&row, select_button);
  }

  for (int i = 0; i < count; i++) {
    GtkWidget *cell = table_cell_new(columns[i].title, columns[i].width, "table-header");
    table_row_insert_cell(&row, cell);
  }

  table_insert_row(table, &row);
}
