#include "widget_utils.h"

void widget_remove_children(GtkWidget *parent) 
{
  if (parent == NULL) return;

  GtkWidget *child = gtk_widget_get_first_child(parent);

  while (child != NULL)
  {
    GtkWidget *next = gtk_widget_get_next_sibling(child);
    
    gtk_widget_unparent(child);

    child = next;
  }
}

void list_remove_rows(GtkWidget *list)
{
  GtkWidget *child = gtk_widget_get_first_child(list);
  while (child != NULL)
  {
    gtk_list_box_remove(GTK_LIST_BOX(list), child);
    child = gtk_widget_get_first_child(list);
  }
}

void table_remove_rows(GtkWidget *grid)
{
  if (grid == NULL) return;

  GtkWidget *item = gtk_widget_get_first_child(grid);
  
  while (item != NULL)
  {
    GtkWidget *next = gtk_widget_get_next_sibling(item);
    
    int row;
    gtk_grid_query_child(GTK_GRID(grid), item, NULL, &row, NULL, NULL);
    
    if (row > 0)  gtk_grid_remove(GTK_GRID(grid), item); // Skip the headers

    item = next;
  }
}
