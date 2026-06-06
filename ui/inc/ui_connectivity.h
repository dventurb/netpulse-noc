#ifndef UI_CONNECTIVITY_H
#define UI_CONNECTIVITY_H

#include "ui_main_window.h"
#include "application.h"
#include "ui_widgets.h"
#include "connectivity.h"
#include <gtk/gtk.h>

typedef enum {
  SOURCE_SELECTION_SEARCH,
  SOURCE_SELECTION_MANUAL
} target_source_selection_t;

typedef struct {
  application_t *application;
  
  GtkWidget *window;
  GtkWidget *container;
  GtkWidget *stack;

  GtkWidget *buttons[6];
  GtkWidget *sidebar_buttons[6];
} ui_connectivity_t;

typedef struct ui_ping_configuration_t {
  application_t *application;

  GtkWidget *list;
  
  GtkWidget *count;
  GtkWidget *timeout;
  GtkWidget *packet_size;

  GtkWidget *manual_ip;

  GtkWidget *terminal;

  char target_ip[IP_MAX];
  target_source_selection_t source;
} ui_ping_configuration_t;

GtkWidget *create_page_connectivity(ui_t *ui);
void ui_ping_set_result(ui_ping_configuration_t *ui_ping, ping_result_t *result);
void ui_ping_update_list(ui_ping_configuration_t *ui_ping, equipment_t *equipments, int count);

#endif
