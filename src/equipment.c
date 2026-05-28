#include "equipment.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Private function
static equipment_node_t *get_middle(equipment_node_t *node);
static equipment_node_t *merge_sort(equipment_node_t *node, callback_fn compare);
static equipment_node_t *merge(equipment_node_t *left, equipment_node_t *right, callback_fn compare);
static equipment_node_t *get_tail(equipment_list_t *list);
static int compare_by_status(const equipment_node_t *left, const equipment_node_t *right);
static int compare_by_location(const equipment_node_t *left, const equipment_node_t *right);

static int compare_by_type(const equipment_node_t *left, const equipment_node_t *right);


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

equipment_node_t *equipment_list_insert(equipment_list_t *list, equipment_t data)
{
  equipment_node_t *new = malloc(sizeof(equipment_node_t));
  if (new == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_list_insert : malloc failed)
    return NULL;
  }

  new->data = data;

  new->data.id = list->next_id++;
  new->data.last_check = time(NULL);

  new->next = NULL;

  if (list->head == NULL)
  {
    new->previous = NULL;

    list->head = new;
    list->tail = new;

    list->count++;

    return new;
  }

  new->previous = list->tail;
  list->tail->next = new;
  list->tail = new;

  list->count++;

  return new;
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

void equipment_update(equipment_t *equipment, equipment_t data)
{
  if (equipment == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_update : NULL argument)
    return;
  }

  strcpy(equipment->name, data.name);
  strcpy(equipment->vendor, data.vendor);
  strcpy(equipment->model, data.model);
  strcpy(equipment->ip_address, data.ip_address);
  strcpy(equipment->mac_address, data.mac_address);
  strcpy(equipment->location, data.location);
  equipment->type = data.type;
  equipment->status = data.status;
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

  equipment->last_check = time(NULL);
}

void equipment_filter(const equipment_list_t *list, equipment_status_t status, equipment_type_t type, equipment_list_t *filtered)
{
  if (list == NULL || filtered == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_filter: NULL arguments)
    return;
  }

  equipment_node_t *node = list->head;

  while (node != NULL)
  {
    if (node->data.status == status && node->data.type == type)
    {
      equipment_node_t *new = equipment_list_insert(filtered, node->data);
      new->data = node->data;
    }

    node = node->next;
  }
}

void equipment_filter_by_status(const equipment_list_t *list, equipment_status_t status, equipment_list_t *filtered)
{
  if (list == NULL || filtered == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_filter_by_status: NULL arguments)
    return;
  }

  equipment_node_t *node = list->head;

  while (node != NULL)
  {
    if (node->data.status == status)
    {
      equipment_node_t *new = equipment_list_insert(filtered, node->data);
      new->data = node->data;
    }

    node = node->next;
  }
}

void equipment_filter_by_type(const equipment_list_t *list, equipment_type_t type, equipment_list_t *filtered)
{
  if (list == NULL || filtered == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_filter_by_type: NULL arguments)
    return;
  }

  equipment_node_t *node = list->head;

  while (node != NULL)
  {
    if (node->data.type == type)
    {
      equipment_node_t *new = equipment_list_insert(filtered, node->data);
      new->data = node->data;
    }
    
    node = node->next;
  }
}

void equipment_list_sort_by_status(equipment_list_t *list)
{
  if (list == NULL || list->head == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_list_sort_by_status : NULL argument)
    return;
  }

  list->head = merge_sort(list->head, compare_by_status);
  list->tail = get_tail(list);
}

void equipment_list_sort_by_location(equipment_list_t *list)
{
  if (list == NULL || list->head == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_list_sort_by_location : NULL argument)
    return;
  }

  list->head = merge_sort(list->head, compare_by_location);
  list->tail = get_tail(list);
}

void equipment_list_sort_by_type(equipment_list_t *list)
{
  if (list == NULL || list->head == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_list_sort_by_type : NULL argument)
    return;
  }

  list->head = merge_sort(list->head, compare_by_type);
  list->tail = get_tail(list);
}

// Fast and Slow Pointer
static equipment_node_t *get_middle(equipment_node_t *node)
{
  equipment_node_t *slow = node;
  equipment_node_t *fast = node;

  while (fast != NULL && fast->next != NULL)
  {
    slow = slow->next;
    fast = fast->next->next;
  }

  return slow; // First element from the right side (left: head to slow->previous; right: slow to tail)
}

static equipment_node_t *merge_sort(equipment_node_t *node, callback_fn compare)
{
  // Base case: list empty or already sorted
  if (node == NULL || node->next == NULL)
  {
    return node;
  }

  equipment_node_t *left = node;
  equipment_node_t *right = get_middle(node);

  right->previous->next = NULL;
  right->previous = NULL;

  left = merge_sort(left, compare);
  right = merge_sort(right, compare);

  return merge(left, right, compare);
}

static equipment_node_t *merge(equipment_node_t *left, equipment_node_t *right, callback_fn compare)
{
  if (left == NULL) return right;
  if (right == NULL) return left;

  equipment_node_t *node;

  if (compare(left, right) >= 0)
  {
    node = left;
    node->next = merge(node->next, right, compare);
  }

  else 
  {
    node = right;
    node->next = merge(left, node->next, compare);
  }

  if (node->next != NULL)
  {
    node->next->previous = node;
  }

  return node;
}

static equipment_node_t *get_tail(equipment_list_t *list)
{
  if (list == NULL || list->head == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] get_tail : NULL argument)
    return NULL;
  }

  equipment_node_t *node = list->head;
  
  while (node->next != NULL)
  {
    node = node->next;
  }

  return node;
}

static int compare_by_status(const equipment_node_t *left, const equipment_node_t *right)
{
  return left->data.status - right->data.status;
}

static int compare_by_location(const equipment_node_t *left, const equipment_node_t *right)
{  
  return strcmp(left->data.location, right->data.location);
}

static int compare_by_type(const equipment_node_t *left, const equipment_node_t *right)
{
  return left->data.type - right->data.type;
}

const char *equipment_status_to_string(equipment_status_t status)
{
  switch (status) 
  {
    case STATUS_MAINTENANCE: return "Maintenance";
    case STATUS_FAILED: return "Failed";
    case STATUS_OPERATIONAL: return "Operational";
    case STATUS_DISABLED: return "Disabled";
    default: return "Unknown";
  }
}

const char *equipment_type_to_string(equipment_type_t type)
{
  switch (type) 
  {
    case TYPE_ROUTER: return "Router";
    case TYPE_FIREWALL: return "Firewall";
    case TYPE_SWITCH: return "Switch";
    case TYPE_ACCESS_POINT: return "Access Point";
    case TYPE_SERVER: return "Server";
    case TYPE_NAS: return "NAS";
    case TYPE_UPS: return "UPS";
    case TYPE_IP_CAMERA: return "IP Camera";
    case TYPE_PRINTER: return "Printer";
    case TYPE_OTHER: return "Other";
    default: return "Other";
  }
}
