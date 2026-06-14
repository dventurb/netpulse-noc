#ifndef APPLICATION_H
#define APPLICATION_H

#include "equipment.h"
#include "incident.h"
#include "sensor.h"
#include "technician.h"
#include "hashmap.h"

typedef struct {
  equipment_list_t equipments;
  
  hashmap_t id_index;
  hashmap_t ip_index;
  hashmap_t mac_index;

  incident_queue_t incidents_pending; // PENDING
  incident_list_t incidents_history; // IN_PROGRESS + CONCLUDED
 
  sensor_array_t sensors;

  technician_list_t technicians;
  technician_t *current_user;

  hashmap_t username_index;
} application_t;

void application_init(application_t *application);
void application_destroy(application_t *application);

#endif
