#ifndef AUTH_H
#define AUTH_H

#include "app.h"
#include <stdbool.h>

typedef enum {
  REGISTER_INVALID_NAME,
  REGISTER_INVALID_USERNAME,
  REGISTER_INVALID_PASSWORD,
  REGISTER_VALID
} register_validate_t;

register_validate_t auth_validate_register(app_t *app, const char *name, const char *username, const char *password);
bool auth_register_new_technician(app_t *app, const char *name, const char *username, const char *password);

#endif
