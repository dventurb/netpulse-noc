#ifndef INFO_CARD_T
#define INFO_CARD_T

#include <gtk/gtk.h>

typedef struct {

  GtkBox   *container;

  GtkLabel *title;
  GtkLabel *subtitle;
  GtkLabel *value;

  GtkImage *icon;

} info_card_t;


info_card_t info_card_new(const char *title, const char *subtitle, const char *value, const char *icon);

void info_card_set_title(info_card_t *card, const char *title);
void info_card_set_subtitle(info_card_t *card, const char *subtitle);
void info_card_set_value(info_card_t *card, const char *value);
void info_card_set_icon(info_card_t *card, const char *icon);


#endif
