#ifndef STATS_CARD_H
#define STATS_CARD_H

#include <gtk/gtk.h>

typedef struct {

  GtkBox *container;

  GtkLabel *title;
  GtkLabel *value;

} stats_card_t;


stats_card_t stats_card_new(const char *title, const char *value, const char *css);

void stats_card_set_title(stats_card_t *card, const char *title);
void stats_card_set_value(stats_card_t *card, const char *value);

#endif
