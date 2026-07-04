#include "toggle_button.h"

toggle_button_t toggle_button_new(const char *text, const char *icon, const char *css)
{
  toggle_button_t button = {0};

  button.widget = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());

  button.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
  gtk_widget_set_halign(GTK_WIDGET(button.container), GTK_ALIGN_CENTER);
  gtk_widget_set_valign(GTK_WIDGET(button.container), GTK_ALIGN_CENTER);

  gtk_button_set_child(GTK_BUTTON(button.widget), GTK_WIDGET(button.container));

  toggle_button_set_icon(&button, icon);
  toggle_button_set_label(&button, text);
  toggle_button_set_css_class(&button, css);

  return button;
}

void toggle_button_remove_child(toggle_button_t *button, GtkWidget *child)
{
  if (child == NULL) return;

  gtk_box_remove(button->container, child);
}

void toggle_button_set_icon(toggle_button_t *button, const char *icon)
{
  if (icon == NULL) 
  {
    button->icon = NULL;
    return;
  }

  if (button->icon != NULL)
  {
    gtk_image_set_from_file(button->icon, icon);
    return;
  }

  button->icon = GTK_IMAGE(gtk_image_new_from_file(icon));
  gtk_box_append(button->container, GTK_WIDGET(button->icon));
}

void toggle_button_set_label(toggle_button_t *button, const char *text)
{
  if (text == NULL) 
  {
    button->label = NULL;
    return;
  }

  if (button->label != NULL)
  {
    gtk_label_set_text(button->label, text);
    return;
  }

  button->label = GTK_LABEL(gtk_label_new(text));
  gtk_box_append(button->container, GTK_WIDGET(button->label));
}

void toggle_button_set_css_class(toggle_button_t *button, const char *css)
{
  if (css == NULL) return;

  gtk_widget_add_css_class(GTK_WIDGET(button->widget), css);
}
