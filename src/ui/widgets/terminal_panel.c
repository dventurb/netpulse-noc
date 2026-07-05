#include "terminal_panel.h"

terminal_panel_t terminal_panel_new(const char *title)
{
  terminal_panel_t terminal = {0};

  terminal.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_widget_set_size_request(header, -1, 44);
  gtk_widget_add_css_class(header, "terminal-header");

  GtkWidget *close_icon = gtk_image_new_from_file("assets/terminal-control-close.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(close_icon), 12);
 
  GtkWidget *minimize_icon = gtk_image_new_from_file("assets/terminal-control-minimize.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(minimize_icon), 12);
  
  GtkWidget *maximize_icon = gtk_image_new_from_file("assets/terminal-control-maximize.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(maximize_icon), 12);

  terminal.copy_button = GTK_BUTTON(action_button_new("Copy", "assets/icon-copy.svg", "terminal-header-button"));
  gtk_widget_set_halign(GTK_WIDGET(terminal.copy_button), GTK_ALIGN_END);
  gtk_widget_set_hexpand(GTK_WIDGET(terminal.copy_button), TRUE);

  terminal.save_button = GTK_BUTTON(action_button_new("Save", "assets/icon-save.svg", "terminal-header-button"));
  gtk_widget_set_halign(GTK_WIDGET(terminal.save_button), GTK_ALIGN_END);

  terminal.title = GTK_LABEL(gtk_label_new(title ? title : ""));
  gtk_widget_add_css_class(GTK_WIDGET(terminal.title), "terminal-header-title");

  gtk_box_append(GTK_BOX(header), close_icon);
  gtk_box_append(GTK_BOX(header), minimize_icon);
  gtk_box_append(GTK_BOX(header), maximize_icon);
  gtk_box_append(GTK_BOX(header), GTK_WIDGET(terminal.title));
  gtk_box_append(GTK_BOX(header), GTK_WIDGET(terminal.copy_button));
  gtk_box_append(GTK_BOX(header), GTK_WIDGET(terminal.save_button));

  GtkWidget *scrolled = gtk_scrolled_window_new();
  gtk_widget_set_size_request(scrolled, -1, 400);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  terminal.text_view = GTK_TEXT_VIEW(gtk_text_view_new());
  gtk_widget_set_size_request(GTK_WIDGET(terminal.text_view), -1, 400);
  gtk_text_view_set_editable(terminal.text_view, FALSE);
  //gtk_text_view_set_cursor_visible(terminal.text_view, FALSE);
  gtk_widget_add_css_class(GTK_WIDGET(terminal.text_view), "terminal-panel");

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), GTK_WIDGET(terminal.text_view));

  terminal.buffer = gtk_text_buffer_new(NULL);
  gtk_text_view_set_buffer(terminal.text_view, terminal.buffer);

  terminal_panel_clear_output(&terminal);

  gtk_box_append(terminal.container, header);
  gtk_box_append(terminal.container, scrolled);

  return terminal;
}

void terminal_panel_set_title(terminal_panel_t *terminal, const char *title)
{
  if (title == NULL || terminal->title == NULL) return;

  gtk_label_set_text(terminal->title, title);
}

void terminal_panel_set_output(terminal_panel_t *terminal, const char *output)
{
  if (terminal->text_view == NULL || terminal->buffer == NULL || output == NULL) return;

  char *utf8_char = g_utf8_make_valid(output, -1);

  GtkTextIter end;
  gtk_text_buffer_get_end_iter(terminal->buffer, &end);

  gtk_text_buffer_insert(terminal->buffer, &end, utf8_char, -1);

  gtk_text_buffer_get_end_iter(terminal->buffer, &end);
  gtk_text_buffer_insert(terminal->buffer, &end, "\n$ ", -1);

  gtk_text_buffer_get_end_iter(terminal->buffer, &end);
  gtk_text_view_scroll_to_iter(terminal->text_view, &end, 0.0f, FALSE, 0.0f, 0.0f);

  g_free(utf8_char);
}

char *terminal_panel_get_output(terminal_panel_t *terminal)
{
  if (terminal->buffer == NULL) return NULL;
  
  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(terminal->buffer, &start);
  gtk_text_buffer_get_end_iter(terminal->buffer, &end);

  return gtk_text_buffer_get_text(terminal->buffer, &start, &end, FALSE);
}

void terminal_panel_clear_output(terminal_panel_t *terminal)
{
  if (terminal->buffer == NULL) return;

  gtk_text_buffer_set_text(terminal->buffer, "$ ", -1);
}
