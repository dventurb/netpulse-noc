#ifndef APPLICATION_H
#define APPLICATION_H

#include "equipment.h"
#include "incident.h"
#include "sensor.h"
#include "config.h"
#include "technician.h"
#include "hashmap.h"

typedef struct {
  equipment_list_t equipments;
  hashmap_t id_index;
  hashmap_t ip_index;
  hashmap_t mac_index;
  incident_queue_t incidents;
  config_stack_t configs;
  sensor_list_t sensors;
  technician_list_t technicians;
  hashmap_t username_index;
  technician_t *current_user;
} application_t;

#endif
