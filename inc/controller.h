#ifndef CONTROLLER_H
#define CONTROLLER_H 

#include <glib.h>
#include "ui_connectivity.h"
#include "macros.h"

typedef struct {
  char ip[STRING_MAX];
  int count;
  int timeout;
  int packet_size;
} ping_params_t;

gboolean on_ping_finished(gpointer data);
void connectivity_controller_ping(ui_ping_configuration_t *ui_ping, const char *ip, int count, int timeout, int packet_size);

#endif
