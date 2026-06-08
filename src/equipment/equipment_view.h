#ifndef EQUIPMENT_VIEW_T
#define EQUIPMENT_VIEW_T

#include <gtk/gtk.h>

#include "equipment.h"
#include "equipment_controller.h"

typedef enum {
  EQUIPMENT_FORM_ADD,
  EQUIPMENT_FORM_EDIT,
  EQUIPMENT_FORM_REMOVE
} equipment_form_mode_t;

typedef struct {
  GtkWindow *dialog;

  GtkWidget   *layout;

  equipment_form_mode_t mode;
} equipment_form_t;

typedef struct equipment_view_t {
  equipment_controller_t *controller;
  
  GtkBox      *container;
  GtkGrid     *table;

  GtkBox      *cards;
  GtkBox      *pagination_bar;

  GtkButton   *add_button;
  GtkButton   *edit_button;
  GtkButton   *remove_button;

  GtkDropDown *sort_filter;
  GtkDropDown *type_filter;
  GtkDropDown *status_filter;

  equipment_form_t form;

} equipment_view_t;

GtkBox *equipment_view_create(equipment_view_t *equipment_view, equipment_controller_t *controller);

void equipment_view_refresh(equipment_view_t *view);
void equipment_view_update_table(equipment_view_t *view, const equipment_t *equipments, int count);
void equipment_view_update_header(equipment_view_t *view);
void equipment_view_update_stats_cards(equipment_view_t *view);

#endif
