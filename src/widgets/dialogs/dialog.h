#ifndef DIALOG_H
#define DIALOG_H 

#include <gtk/gtk.h>

typedef struct {
  const char *label;
  const char *icon;
  const char *css;
  
  GCallback callback;
  
  void *data;
} dialog_action_t;

typedef struct {
  GtkWidget *window;
  GtkWidget *form;

  const char *title;
  
  dialog_action_t action;
} dialog_config_t;

GtkWidget *dialog_new(dialog_config_t config);

#endif
