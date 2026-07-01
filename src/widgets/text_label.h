#ifndef TEXT_LABEL_H
#define TEXT_LABEL_H

#include <gtk/gtk.h>

typedef enum {
  TEXT_LABEL_TITLE,
  TEXT_LABEL_DESCRIPTION
} text_label_style_t;

GtkWidget *text_label_new(const char *text, text_label_style_t style);

#endif
