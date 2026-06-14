#ifndef APP_H
#define APP_H

#include "app_data.h"
#include "app_windows.h"

typedef enum {
  APP_STATE_LOGIN,
  APP_STATE_MAIN
} app_state_t;

typedef struct app_t {
  app_data_t      data; 
  app_windows_t   windows;

  app_state_t     state; 

  GtkApplication *gtk_app;
} app_t;


void app_init(app_t *app);
void app_destroy(app_t *app);

int app_launcher(app_t *app, int argc, char **argv);

#endif
