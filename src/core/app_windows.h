#ifndef APP_WINDOWS_H
#define APP_WINDOWS_H

#include "main_window.h"
#include "login_window.h"

typedef struct {
  main_window_t  *main;
  login_window_t *login;

} app_windows_t;


void app_windows_init(app_windows_t *windows);

void app_windows_create_login(app_windows_t *windows, app_t *app);
void app_windows_create_main(app_windows_t *windows, app_t *app);

void app_windows_switch(app_windows_t *windows, app_t *app);

#endif
