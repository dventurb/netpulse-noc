#ifndef INPUT_FIELD_H
#define INPUT_FIELD_H

#include <gtk/gtk.h>

typedef struct {
  GtkBox   *container;
  GtkEntry *entry;
} input_field_t;

input_field_t input_field_new(const char *text, const char *placeholder, const char *icon_file);

#endif
