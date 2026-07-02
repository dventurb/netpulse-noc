#ifndef SCREEN_H
#define SCREEN_H

#include <gtk/gtk.h>

typedef struct screen_t {
  GtkWidget *root;

  void (*refresh)(struct screen_t *screen);
  void (*destroy)(struct screen_t *screen);

} screen_t;

GtkWidget *screen_get_root(screen_t *screen);
void screen_refresh(screen_t *screen);
void screen_destroy(screen_t *screen);

#endif
