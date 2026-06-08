#ifndef CONNECTIVITY_CONTROLLER_H
#define CONNECTIVITY_CONTROLLER_H 

#include <glib.h>

#include "application.h"

#include "macros.h"
#include "pagination.h"

// forward declaration to resolve circular dependencies
typedef struct connectivity_view_t connectivity_view_t;

typedef enum {
  SOURCE_SEARCH,
  SOURCE_MANUAL
} target_source_t;

typedef enum {
  PING_INVALID_IP_ADDRESS,
  PING_INVALID_COUNT,
  PING_INVALID_TIMEOUT,
  PING_INVALID_PACKET_SIZE,
  PING_OK
} ping_validation_t;

typedef struct {
  char ip[IP_MAX];
  int count;
  int timeout;
  int packet_size;
} ping_params_t;

typedef struct {
  application_t *app;

  connectivity_view_t *view;

  char ip[IP_MAX];
  target_source_t source;

  equipment_t *selected_equipment;
} connectivity_controller_t;


void connectivity_controller_init(connectivity_controller_t *controller, connectivity_view_t *view, void *data);

void connectivity_controller_ping(connectivity_controller_t *controller, const char *count, const char *timeout, const char *packet_size);
void connectivity_controller_search_equipment(connectivity_controller_t *controller, const char *text);

void connectivity_controller_set_source_selection(connectivity_controller_t *controller, target_source_t source);
void connectivity_controller_set_equipment(connectivity_controller_t *controller, equipment_t *equipment);
void connectivity_controller_set_ip_address(connectivity_controller_t *controller, const char *ip_address);
void connectivity_controller_set_ip_from_source(connectivity_controller_t *controller, const char *ip_address);

ping_validation_t connectivity_controller_validate_ping(const char *ip, const char *count, const char *timeout, const char *packet_size);

void connectivity_controller_create_incident(connectivity_controller_t *controller, const equipment_t *equipment);

gboolean on_ping_finished(gpointer data);

#endif
