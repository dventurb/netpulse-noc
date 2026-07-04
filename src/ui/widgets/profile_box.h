#ifndef PROFILE_BOX_H
#define PROFILE_BOX_H

#include <gtk/gtk.h>

typedef struct {

  GtkBox   *container;
  GtkLabel *name;
  GtkImage *avatar;

} profile_box_t;

profile_box_t profile_box_new(const char *name, const char *icon_file);
// TODO: profile_box_set_name
// TODO: profile_box_set_avatar

#endif
