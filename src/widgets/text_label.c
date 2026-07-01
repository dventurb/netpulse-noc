#include "text_label.h"

GtkWidget *text_label_new(const char *text, text_label_style_t style)
{
  if (text == NULL) return NULL;

  GtkWidget *label = gtk_label_new(text);

  switch (style) 
  {
    case TEXT_LABEL_TITLE:
      gtk_widget_add_css_class(label, "remove-primary-label");
      break;

    case TEXT_LABEL_DESCRIPTION:
      gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
      gtk_widget_add_css_class(label, "remove-secundary-label");
      break;

    default: break;
  }

  return label;
}
