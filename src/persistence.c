#include "persistence.h"

#include <stdio.h>
#include "hashmap.h"

void load_equipments(application_t *application, const char *filepath)
{
  if (application == NULL || filepath == NULL) return;
  
  equipment_list_t *list = &application->equipments;

  FILE *file = fopen(filepath, "rb");
  if (file == NULL) return;

  equipment_t data;

  while (fread(&data, sizeof(equipment_t), 1, file))
  {
    equipment_node_t *node = equipment_list_reinsert(list, data);

    char id[ID_MAX];
    equipment_format_id(node->data.id, id);

    hashmap_insert(&application->id_index, id, node);
    hashmap_insert(&application->ip_index, node->data.ip_address, node);
    hashmap_insert(&application->mac_index, node->data.mac_address, node);
  }
  
  fclose(file);
}

void load_incidents(incident_queue_t *queue, incident_list_t *list, const char *filepath)
{
  if (queue == NULL || list == NULL || filepath == NULL) return;

  FILE *file = fopen(filepath, "rb");
  if (file == NULL) return;

  incident_t data;

  while (fread(&data, sizeof(incident_t), 1, file))
  {
    if (data.status == INCIDENT_PENDING)
      incident_queue_requeue(queue, data);
    else 
      incident_list_reinsert(list, data);
  }

  fclose(file);
}

void save_equipments(equipment_list_t *list, const char *filepath)
{
  if (list == NULL || filepath == NULL) return;

  FILE *file = fopen(filepath, "wb");
  if (file == NULL) return;

  equipment_node_t *node = list->head;
  while (node != NULL)
  {
    equipment_t data = node->data;
    fwrite(&data, sizeof(equipment_t), 1, file);

    node = node->next;
  }

  fclose(file);
}

void save_incidents(incident_queue_t *queue, incident_list_t *list, const char *filepath)
{
  if (queue == NULL || list == NULL || filepath == NULL) return;

  FILE *file = fopen(filepath, "wb");
  if (file == NULL) return;

  incident_node_t *node = queue->front;
  while (node != NULL)
  {
    incident_t data = node->data;
    fwrite(&data, sizeof(incident_t), 1, file);

    node = node->next;
  }

  node = list->head;
  while (node != NULL)
  {
    incident_t data = node->data;
    fwrite(&data, sizeof(incident_t), 1, file);

    node = node->next;
  }

  fclose(file);
}
