#include "equipment.h"

#include <stdlib.h>

void equipment_list_init(equipment_list_t *list)
{
  list->head = NULL;
  list->tail = NULL;
  list->count = 0;
  list->next_id = 1;
}

void equipment_list_destroy(equipment_list_t *list)
{
  equipment_node_t *node = list->head;
  
  while (node != NULL)
  {
    equipment_node_t *next = node->next;
    free(node);
    node = next;
  }

  list->head = NULL;
  list->tail = NULL;
  list->count = 0;
  list->next_id = 0;
}

void equipment_list_insert(equipment_list_t *list, equipment_t data)
{
  equipment_node_t *new = malloc(sizeof(equipment_node_t));
  if (new == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_list_insert : malloc failed)
    return;
  }

  new->data = data;
  new->next = NULL;

  if (list->head == NULL)
  {
    new->data.id = list->next_id++;
    new->previous = NULL;

    list->head = new;
    list->tail = new;

    list->count++;

    return;
  }
  
  new->data.id = list->next_id++;

  new->previous = list->tail;
  list->tail->next = new;
  list->tail = new;

  list->count++;
}

void equipment_list_remove(equipment_list_t *list, equipment_node_t *node) 
{
  if (list->head == NULL || node == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_list_remove : NULL arguments)
    return;
  }
  
  if (node != list->head) node->previous->next = node->next;

  else list->head = node->next;

  if (node->next != NULL) node->next->previous = node->previous;

  else list->tail = node->previous;

  free(node);

  list->count--;
}

void equipment_update_name(equipment_t equipment, const char *name)
{
  if (equipment == NULL || name == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_update_name : NULL arguments)
    return;
  }

  snprintf(equipment->name, STRING_MAX, "%s", name);
}

void equipment_update_type(equipment_t *equipment, const char *type)
{
  if (equipment == NULL || type == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_update_type : NULL arguments)
    return;
  }

  snprintf(equipment->type, STRING_MAX, "%s", type);
}

void equipment_update_vendor(equipment_t *equipment, const char *vendor)
{
  if (equipment == NULL || vendor == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_update_vendor : NULL arguments)
    return;
  }

  snprintf(equipment->vendor, STRING_MAX, "%s", vendor);
}

void equipment_update_model(equipment *equipment, const char *model)
{
  if (equipment == NULL || model == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_update_model : NULL arguments)
    return;
  }

  snprintf(equipment->model, STRING_MAX, "%s", model);
}

void equipment_update_ip_address(equipment_t *equipment, const char *ip_address)
{
  if (equipment == NULL || ip_address == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_update_ip_address : NULL arguments)
    return;
  }

  snprintf(equipment->ip_address, IP_MAX, "%s", ip_address);
}

void equipment_update_mac_address(equipment_t *equipment, const char *mac_address)
{
  if (equipment == NULL || mac_address == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_update_mac_address : NULL arguments)
    return;
  }

  snprintf(equipment->mac_address, MAC_MAX, "%s", mac_address);
}

void equipment_update_location(equipment_t *equipment, const char *location)
{
  if (equipment == NULL || location == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_update_location : NULL arguments)
    return;
  }

  snprintf(equipment->location, STRING_MAX, "%s", location);
}

void equipment_update_status(equipment_t *equipment, equipment_status_t status)
{
  if (equipment == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_update_status : NULL arguments)
    return;
  }

  equipment->status = status;
}

void equipment_update_last_check(equipment_t *equipment)
{
  if (equipment == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_update_last_check: NULL arguments)
    return;
  }

  // TODO: Implement a get_datetime()
}


int equipment_filter_by_status(const equipment_list_t *list, equipment_status_t status, equipment_t *equipments)
{
  if (list == NULL || equipments == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_filter_by_status: NULL arguments)
    return 0;
  }

  equipment_node_t *node = list->head;
  int i = 0;

  while (node != NULL && i < list->count)
  {
    if (node->data.status == status)
    {
      equipments[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}

int equipment_filter_by_type(const equipment_list_t *list, const char *type, equipment_t *equipments)
{
  if (list == NULL || equipments == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_filter_by_type: NULL arguments)
    return 0;
  }

  equipment_node_t *node = list->head;
  int i = 0;

  while (node != NULL && i < list->count)
  {
    if (strcmp(node->data.type, type) == 0)
    {
      equipments[i] = node->data;
      i++;
    }
    
    node = node->next;
  }

  return i;
}
