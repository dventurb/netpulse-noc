#ifndef UNIT_FIELD_H
#define UNIT_FIELD_H

#include <gtk/gtk.h>

typedef struct {
  GtkBox   *container;
  GtkEntry *entry;
  GtkLabel *unit_label;
} unit_field_t;

unit_field_t unit_field_new(const char *text, const char *placeholder, const char *unit);

#endif
