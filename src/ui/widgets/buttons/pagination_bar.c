#include "pagination_bar.h"

#include "action_button.h"

static void pagination_bar_remove_buttons(pagination_bar_t *bar);
static int pagination_bar_get_button_number(pagination_bar_t *bar, GtkButton *button);

// Callbacks
static void on_previous_page_button_clicked(GtkButton *button, gpointer data);
static void on_next_page_button_clicked(GtkButton *button, gpointer data);
static void on_page_button_clicked(GtkButton *button, gpointer data);


pagination_bar_t pagination_bar_new(pagination_t *pagination)
{
  pagination_bar_t bar = {0};

  bar.pagination = pagination;
  bar.buttons_total = 0;

  bar.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4));
  gtk_widget_set_size_request(GTK_WIDGET(bar.container), -1, 64);
  gtk_widget_set_hexpand(GTK_WIDGET(bar.container), TRUE);
  gtk_widget_set_halign(GTK_WIDGET(bar.container), GTK_ALIGN_END);

  bar.previous_button = GTK_BUTTON(action_button_new(NULL, "assets/left-arrow.svg", "arrow-page"));
  gtk_widget_set_margin_top(GTK_WIDGET(bar.previous_button), 16);
  gtk_widget_set_margin_bottom(GTK_WIDGET(bar.previous_button), 16);
  gtk_widget_set_size_request(GTK_WIDGET(bar.previous_button), 32, 32);
  
  bar.next_button = GTK_BUTTON(action_button_new(NULL, "assets/right-arrow.svg", "arrow-page"));
  gtk_widget_set_margin_top(GTK_WIDGET(bar.next_button), 16);
  gtk_widget_set_margin_bottom(GTK_WIDGET(bar.next_button), 16);
  gtk_widget_set_margin_end(GTK_WIDGET(bar.next_button), 24);
  gtk_widget_set_size_request(GTK_WIDGET(bar.next_button), 32, 32);

  gtk_box_append(bar.container, GTK_WIDGET(bar.previous_button));
  gtk_box_append(bar.container, GTK_WIDGET(bar.next_button));

  return bar;
}

void pagination_bar_destroy(pagination_bar_t *bar)
{
  if (bar->page_buttons == NULL) return;
  free(bar->page_buttons);
}

void pagination_bar_refresh(pagination_bar_t *bar)
{
  pagination_bar_remove_buttons(bar);
  
  int start, end;
  pagination_get_range(*bar->pagination, &start, &end);

  int total = end - start + 1;
  if (total == 0) return;

  bar->page_buttons = malloc(sizeof(GtkButton *) * total);
  if (bar->page_buttons == NULL) return;

  for (int i = total - 1; i >= 0; i--) 
  {
    int number = i + start;

    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%d", number + 1);

    bar->page_buttons[i] = GTK_BUTTON(pagination_button_new(bar, buffer,  number));

    gtk_box_insert_child_after(bar->container, GTK_WIDGET(bar->page_buttons[i]),GTK_WIDGET(bar->previous_button));

    bar->buttons_total++;
  }
}

void pagination_bar_set_callback(pagination_bar_t *bar, pagination_bar_callback callback, void *data)
{
  if (callback) bar->callback = callback;
  if (data) bar->data = data;

  g_signal_connect(bar->previous_button, "clicked", G_CALLBACK(on_previous_page_button_clicked), bar);
  g_signal_connect(bar->next_button, "clicked", G_CALLBACK(on_next_page_button_clicked), bar);
}

static void pagination_bar_remove_buttons(pagination_bar_t *bar)
{
  if (bar->page_buttons == NULL) return;

  for (int i = 0; i < bar->buttons_total; i++) 
  {
    gtk_box_remove(bar->container, GTK_WIDGET(bar->page_buttons[i]));
    bar->page_buttons[i] = NULL;
  }

  bar->buttons_total = 0;
  free(bar->page_buttons);
  bar->page_buttons = NULL;
}

GtkWidget *pagination_button_new(pagination_bar_t *bar, char *text, int number)
{
  GtkWidget *button = action_button_new(text, NULL, "default-page");
  gtk_widget_set_margin_top(button, 16);
  gtk_widget_set_margin_bottom(button, 16);
  gtk_widget_set_size_request(button, 32, 32);

  g_signal_connect(button, "clicked", G_CALLBACK(on_page_button_clicked), bar);

  if (number == bar->pagination->current_page) 
    gtk_widget_add_css_class(button, "active-page");

  return button;
}

static int pagination_bar_get_button_number(pagination_bar_t *bar, GtkButton *button)
{
  int start, end;
  pagination_get_range(*bar->pagination, &start, &end);

  for (int i = 0; i < bar->buttons_total; i++) 
  {
    if (bar->page_buttons[i] == button) return start + i;
  }

  return 0;
}

static void on_previous_page_button_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  pagination_bar_t *bar = (pagination_bar_t *)data;

  if (bar->pagination->current_page == 0) return;

  pagination_previous(bar->pagination);

  pagination_bar_refresh(bar);

  if (bar->callback && bar->data)
    bar->callback(bar->data);
}

static void on_next_page_button_clicked(GtkButton *button, gpointer data)
{
  (void)button;

  pagination_bar_t *bar = (pagination_bar_t *)data;
  
  int total_pages = pagination_total_pages(*bar->pagination, bar->pagination->total_items);
  if (bar->pagination->current_page == total_pages) return;

  pagination_next(bar->pagination);

  pagination_bar_refresh(bar);

  if (bar->callback && bar->data)
    bar->callback(bar->data);
}

static void on_page_button_clicked(GtkButton *button, gpointer data)
{
  pagination_bar_t *bar = (pagination_bar_t *)data;

  int number = pagination_bar_get_button_number(bar, button);

  if (number == bar->pagination->current_page) return;

  pagination_set_page_number(bar->pagination, number);
  pagination_bar_refresh(bar);

  if (bar->callback && bar->data)
    bar->callback(bar->data);
}
