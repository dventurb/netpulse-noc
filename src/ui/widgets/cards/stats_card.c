#include "stats_card.h"

#include "macros.h"

stats_card_t stats_card_new(const char *title, const char *value, const char *css)
{
  stats_card_t card = {0};

  card.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
  gtk_widget_set_hexpand(GTK_WIDGET(card.container), TRUE);
  gtk_widget_add_css_class(GTK_WIDGET(card.container), "stats-card");
  gtk_widget_add_css_class(GTK_WIDGET(card.container), css ? css : "");

  card.title = GTK_LABEL(gtk_label_new(title ? title : ""));
  gtk_widget_set_halign(GTK_WIDGET(card.title), GTK_ALIGN_START);
  gtk_widget_add_css_class(GTK_WIDGET(card.title), "stats-card-title");

  card.value = GTK_LABEL(gtk_label_new(value ? value : ""));
  gtk_widget_set_halign(GTK_WIDGET(card.value), GTK_ALIGN_START);
  gtk_widget_add_css_class(GTK_WIDGET(card.value), "stats-card-value");

  gtk_box_append(card.container, GTK_WIDGET(card.title));
  gtk_box_append(card.container, GTK_WIDGET(card.value));

  return card;
}

void stats_card_set_title(stats_card_t *card, const char *title)
{
  if (card->title == NULL || title == NULL) return;

  gtk_label_set_text(card->title, title);
}

void stats_card_set_value(stats_card_t *card, const char *value)
{
  if (card->value == NULL || value == NULL) return;

  gtk_label_set_text(card->value, value);
}
