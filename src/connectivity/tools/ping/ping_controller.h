#ifndef PING_CONTROLLER_T 
#define PING_CONTROLLER_T

#include <glib.h>

#include <stdbool.h>

#include "connectivity.h"
#include "app_data.h"
#include "macros.h"


typedef struct ping_view_t ping_view_t;


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

  bool is_all;

} ping_params_t;

typedef struct {

  char status[STRING_MAX];
  
  char loss_subtitle[STRING_MAX];
  char loss_value[STRING_MAX];

  char latency[STRING_MAX];
  
  char execution_value[STRING_MAX];
  char execution_subtitle[STRING_MAX];

} ping_stats_t;

typedef struct {

  app_data_t *data;

  ping_view_t *view;

  char ip[IP_MAX];
  target_source_t source;

  ping_result_t *result;

  equipment_t *selected_equipment;

} ping_controller_t;


void ping_controller_init(ping_controller_t *controller, ping_view_t *view, app_data_t *data);

void ping_controller_run_ping(ping_controller_t *controller, const char *count, const char *timeout, const char *packet_size);
void ping_controller_run_ping_all_equipments(ping_controller_t *controller);

void ping_controller_search_equipment(ping_controller_t *controller, const char *text);

void ping_controller_set_source_selection(ping_controller_t *controller, target_source_t source);
void ping_controller_set_equipment(ping_controller_t *controller, equipment_t *equipment);
void ping_controller_set_ip_address(ping_controller_t *controller, const char *ip_address);
void ping_controller_set_ip_from_source(ping_controller_t *controller, const char *ip_address);

void ping_controller_save_file(ping_controller_t *controller, const char *filepath);

ping_validation_t ping_controller_validate_fields(const char *ip, const char *count, const char *timeout, const char *packet_size);

void ping_controller_create_equipment_incident(ping_controller_t *controller, const equipment_t *equipment);

void ping_controller_get_stats(ping_controller_t *controller, ping_stats_t *stats);

gboolean on_run_ping_finished(gpointer data);
gboolean on_run_ping_all_equipments_finished(gpointer data);

#endif
