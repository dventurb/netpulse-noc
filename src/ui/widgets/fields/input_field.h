#ifndef INPUT_FIELD_H
#define INPUT_FIELD_H

#include <gtk/gtk.h>

#include <stdbool.h>

typedef struct {
  GtkBox   *container;
  GtkEntry *entry;
} input_field_t;

input_field_t input_field_new(const char *text, const char *placeholder, const char *icon_file);
void input_field_set_error(input_field_t *field, bool error);

#endif
