#ifndef APP_DATA_H
#define APP_DATA_H

#include "equipment.h"
#include "incident.h"
#include "sensor.h"
#include "technician.h"
#include "hashmap.h"

typedef struct {
  equipment_list_t  equipments;
  
  hashmap_t         id_index;
  hashmap_t         ip_index;
  hashmap_t         mac_index;

  incident_queue_t  incidents_pending; // PENDING
  incident_list_t   incidents_history; // IN_PROGRESS + CONCLUDED
 
  sensor_array_t    sensors;

  technician_list_t technicians;
  technician_t      *current_user;

  hashmap_t         username_index;
} app_data_t;


void app_data_init(app_data_t *data);
void app_data_destroy(app_data_t *data);

#endif
