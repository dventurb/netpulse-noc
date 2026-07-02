#ifndef DROPDOWN_FIELD_H
#define DROPDOWN_FIELD_H

#include <gtk/gtk.h>

typedef struct {
  GtkBox      *container;
  GtkDropDown *dropdown;
} dropdown_field_t;

dropdown_field_t dropdown_field_new(const char *text, const char* const *strings);

#endif
