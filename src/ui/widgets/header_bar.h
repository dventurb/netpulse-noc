#ifndef HEADER_BAR_H
#define HEADER_BAR_H

#include <gtk/gtk.h>

#include "profile_box.h"

typedef struct {

  GtkBox        *container;
  GtkPicture    *logo;

  profile_box_t profile_box;

} header_bar_t;

header_bar_t header_bar_new(const char *logo, const char *name, const char *avatar);

// TODO:header_bar_set_avatar
// TODO: header_bar_set_name

#endif
