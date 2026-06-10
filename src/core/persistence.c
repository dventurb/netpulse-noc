#include "persistence.h"

#include "hashmap.h"
#include <stdlib.h>

void load_equipments(application_t *application, const char *filepath)
{
  if (application == NULL || filepath == NULL) return;
  
  equipment_list_t *list = &application->equipments;

  FILE *file = fopen(filepath, "rb");
  if (file == NULL) return;

  equipment_t data = {0};

  while (fread(&data, sizeof(equipment_t), 1, file))
  {
    configuration_stack_init(&data.configs);

    load_configurations(&data, file);

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

void load_configurations(equipment_t *equipment, FILE *file)
{
  if (equipment == NULL || file == NULL) return;

  int count;
  fread(&count, sizeof(count), 1, file);

  if (count == 0) return;

  configuration_t *temp = malloc(sizeof(configuration_t) * count);
  if (temp == NULL) return;

  fread(temp, sizeof(configuration_t), count, file);

  for (int i = count - 1; i >= 0; i--)
  {
    configuration_stack_push(&equipment->configs, temp[i]);
  }

  free(temp);
}

void save_equipments(equipment_list_t *list, const char *filepath)
{
  if (list == NULL || filepath == NULL) return;

  FILE *file = fopen(filepath, "wb");
  if (file == NULL) return;

  equipment_node_t *node = list->head;
  while (node != NULL)
  {
    fwrite(&node->data, sizeof(equipment_t), 1, file);

    save_configurations(&node->data, file);

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

void save_configurations(equipment_t *equipment, FILE *file)
{
  if (equipment == NULL || file == NULL) return;

  int count = equipment->configs.count;
  fwrite(&count, sizeof(count), 1, file);

  configuration_node_t *node = equipment->configs.top;

  while (node != NULL)
  {
    fwrite(&node->data, sizeof(configuration_t), 1, file);

    node = node->next;
  }
}
