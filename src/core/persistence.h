#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "application.h"
#include "equipment.h"
#include "incident.h"

#include <stdio.h>

void load_equipments(application_t *application, const char *filepath);
void load_incidents(incident_queue_t *queue, incident_list_t *list, const char *filepath);
void load_configurations(equipment_t *equipment, FILE *file);

void save_equipments(equipment_list_t *list, const char *filepath);
void save_incidents(incident_queue_t *queue, incident_list_t *list, const char *filepath);
void save_configurations(equipment_t *equipment, FILE *file);

#endif
