#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "application.h"
#include "equipment.h"
#include "incident.h"

#include <stdio.h>

void load_equipments(application_t *application);
void load_incidents(incident_queue_t *queue, incident_list_t *list);

void save_equipments(equipment_list_t *list);
void save_incidents(incident_queue_t *queue, incident_list_t *list);

#endif
