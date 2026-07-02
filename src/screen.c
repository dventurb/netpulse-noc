#include "screen.h"

GtkWidget *screen_get_root(screen_t *screen)
{
  if (screen) return screen->root;
  return NULL;
}

void screen_refresh(screen_t *screen)
{
  if (screen && screen->refresh)
    screen->refresh(screen);
}

void screen_destroy(screen_t *screen)
{
  if (screen && screen->destroy) 
    screen->destroy(screen);
}
