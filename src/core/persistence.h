#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "app_data.h"
#include "equipment.h"
#include "incident.h"
#include "technician.h"
#include "sensor.h"

#include <stdio.h>

void load_technicians(app_data_t *data);
void load_equipments(app_data_t *data);
void load_incidents(incident_queue_t *queue, incident_list_t *list);
void load_sensors(sensor_array_t *array);

void save_technicians(technician_list_t *list);
void save_equipments(equipment_list_t *list);
void save_incidents(incident_queue_t *queue, incident_list_t *list);

#endif
