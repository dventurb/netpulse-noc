#ifndef TABLE_H
#define TABLE_H 

#include <gtk/gtk.h>

#include "pagination_bar.h"

typedef struct {

  GtkBox *container;

  GtkGrid *widget;

  GtkScrolledWindow *scrolled;

  pagination_bar_t pagination_bar;

  int current_row;

} table_t;

typedef struct {

  const char *title;
  int width;

} table_column_t;

typedef struct {

  GtkWidget **cells;

  int count;
  int current;

} table_row_t;


table_t table_new(table_column_t *columns, const int count, bool has_checkbox);
void table_destroy(table_t *table);

void table_set_pagination(table_t *table, pagination_t *pagination, pagination_bar_callback callback, void *data);

void table_insert_row(table_t *table, table_row_t *row);
void table_refresh(table_t *table);
void table_clear_rows(table_t *table);

table_row_t table_row_new(int count);

void table_row_insert_cell(table_row_t *row, GtkWidget *cell);

#endif
