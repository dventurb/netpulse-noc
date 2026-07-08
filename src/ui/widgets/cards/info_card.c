#include "info_card.h"

info_card_t info_card_new(const char *title, const char *icon, const char *value, const char *subtitle)
{
  info_card_t card = {0};

  card.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
  gtk_widget_add_css_class(GTK_WIDGET(card.container), "card-container");
  
  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  
  card.title = GTK_LABEL(gtk_label_new(title));
  gtk_widget_set_halign(GTK_WIDGET(card.title), GTK_ALIGN_START);
  gtk_widget_add_css_class(GTK_WIDGET(card.title), "card-title");

  GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_hexpand(spacer, TRUE);

  card.icon = GTK_IMAGE(gtk_image_new_from_file(icon));
  gtk_widget_set_halign(GTK_WIDGET(card.icon), GTK_ALIGN_END);

  gtk_box_append(GTK_BOX(header), GTK_WIDGET(card.title));
  gtk_box_append(GTK_BOX(header), spacer);
  gtk_box_append(GTK_BOX(header), GTK_WIDGET(card.icon));

  card.value = GTK_LABEL(gtk_label_new(value));
  gtk_widget_set_halign(GTK_WIDGET(card.value), GTK_ALIGN_START);
  gtk_widget_add_css_class(GTK_WIDGET(card.value), "card-value");
  gtk_widget_set_tooltip_text(GTK_WIDGET(card.value), value);

  card.subtitle = GTK_LABEL(gtk_label_new(subtitle));
  gtk_widget_set_vexpand(GTK_WIDGET(card.subtitle), TRUE);
  gtk_widget_set_halign(GTK_WIDGET(card.subtitle), GTK_ALIGN_START);
  gtk_widget_set_valign(GTK_WIDGET(card.subtitle), GTK_ALIGN_END);
  gtk_widget_set_margin_bottom(GTK_WIDGET(card.subtitle), 5);
  gtk_widget_add_css_class(GTK_WIDGET(card.subtitle), "card-subtitle");
  gtk_label_set_width_chars(card.subtitle, 1);
  gtk_label_set_ellipsize(card.subtitle, PANGO_ELLIPSIZE_END);
  gtk_widget_set_tooltip_text(GTK_WIDGET(card.subtitle), subtitle);

  gtk_box_append(card.container, header);
  gtk_box_append(card.container, GTK_WIDGET(card.value));
  gtk_box_append(card.container, GTK_WIDGET(card.subtitle));

  return card;
}

void info_card_set_title(info_card_t *card, const char *title)
{
  if (card->title == NULL || title == NULL) return;

  gtk_label_set_text(card->title, title);
}

void info_card_set_subtitle(info_card_t *card, const char *subtitle)
{
  if (card->subtitle == NULL || subtitle == NULL) return;

  gtk_label_set_text(card->subtitle, subtitle);
  gtk_widget_set_tooltip_text(GTK_WIDGET(card->subtitle), subtitle);
}

void info_card_set_value(info_card_t *card, const char *value)
{
  if (card->value == NULL || value == NULL) return;

  gtk_label_set_text(card->value, value);
  gtk_widget_set_tooltip_text(GTK_WIDGET(card->value), value);
}

void info_card_set_icon(info_card_t *card, const char *icon)
{
  if (card->icon == NULL || icon == NULL) return;

  gtk_image_set_from_file(card->icon, icon);
}
