#include "auth.h"

#include "macros.h"
#include "technician.h"
#include "persistence.h"

#include <sodium.h>

register_validation_t auth_validate_register(app_t *app, const char *name, const char *username, const char *password)
{
  hashmap_t *username_index = &app->data.username_index;

  if (name == NULL || strlen(name) < NAME_MIN) 
    return REGISTER_INVALID_NAME;
  if (username == NULL || strlen(username) < USERNAME_MIN) 
    return REGISTER_INVALID_USERNAME;
  if (technician_exists_by_username(username_index, username)) 
    return REGISTER_INVALID_USERNAME;
  if (password == NULL || strlen(password) < PASSWORD_MIN) 
    return REGISTER_INVALID_PASSWORD;
  else 
    return REGISTER_VALID;
}

bool auth_register_new_technician(app_t *app, const char *name, const char *username, const char *password)
{
  technician_list_t *list = &app->data.technicians;
  hashmap_t *hashmap = &app->data.username_index;

  technician_t new = {0};

  if (crypto_pwhash_str(new.password_hash, password, strlen(password), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) return false;

  snprintf(new.name, STRING_MAX, "%s", name);
  snprintf(new.username, USERNAME_MAX, "%s", username);
  new.role = ROLE_TECHNICIAN;
  snprintf(new.avatar_path, STRING_MAX, "%s", "assets/identicon.svg");  
  new.is_active = true;

  technician_node_t *node = technician_list_insert(list, new);
  hashmap_insert(hashmap, username, node);
  
  save_technicians(list);

  app->state = APP_STATE_MAIN;
  app->data.current_user = &node->data;

  return true;
}

login_validation_t auth_login(app_t *app, const char *username, const char *password)
{
  hashmap_t *hashmap = &app->data.username_index;

  technician_node_t *node = (technician_node_t *)hashmap_get(hashmap, username);
  if (node == NULL) return LOGIN_INVALID_USERNAME;

  if (crypto_pwhash_str_verify(node->data.password_hash, password, strlen(password)) != 0) return LOGIN_INVALID_PASSWORD;

  app->state = APP_STATE_MAIN;
  app->data.current_user = &node->data;

  return LOGIN_SUCCESSFUL;
}
