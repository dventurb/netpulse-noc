#include "equipment.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

// Private function
static equipment_node_t *get_middle(equipment_node_t *node);
static equipment_node_t *merge_sort(equipment_node_t *node, callback_fn compare);
static equipment_node_t *merge(equipment_node_t *left, equipment_node_t *right, callback_fn compare);
static equipment_node_t *get_tail(equipment_list_t *list);
static int compare_by_status(const equipment_node_t *left, const equipment_node_t *right);
static int compare_by_location(const equipment_node_t *left, const equipment_node_t *right);

static int compare_by_type(const equipment_node_t *left, const equipment_node_t *right);


static const char *filepath = "data/report.txt";


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

    configuration_stack_destroy(&node->data.configs);
    free(node);
    node = next;
  }

  list->head = NULL;
  list->tail = NULL;
  list->count = 0;
  list->next_id = 0;
}

void equipment_list_clear(equipment_list_t *list)
{
  if (list == NULL) return;

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
  configuration_stack_init(&new->data.configs);

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

equipment_node_t *equipment_list_reinsert(equipment_list_t *list, equipment_t data)
{
  equipment_node_t *new = malloc(sizeof(equipment_node_t));
  if (new == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_list_reinsert : malloc failed)
    return NULL;
  }

  new->data = data;
  new->next = NULL;

  if (data.id >= list->next_id) list->next_id = data.id + 1;

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

void equipment_list_clone(equipment_list_t *source, equipment_list_t *destination)
{
  if (source == NULL || destination == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] equipment_list_clone : NULL arguments)
    return;
  }

  equipment_node_t *node = source->head;
  
  while (node != NULL) 
  {
    equipment_list_reinsert(destination, node->data);
    
    configuration_stack_init(&destination->tail->data.configs);
    configuration_stack_clone(&node->data.configs, &destination->tail->data.configs);

    node = node->next;
  }
}

equipment_t *equipment_list_in_range(equipment_list_t *list, int start, int end, int *count)
{
  if (list == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_list_in_range : NULL argument)
    return NULL;
  }

  end = (end < list->count) ? end : list->count;

  int size = end - start;
  if (size <= 0) return NULL;

  equipment_t *equipments = malloc(sizeof(equipment_t) * size);
  if (equipments == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_list_in_range : malloc failed)
    return NULL;
  }

  equipment_node_t *node = list->head;

  int i = 0;
  while (node != NULL && i < start)
  {
    node = node->next;
    i++;
  }

  int j = 0;
  while (node != NULL && i < end)
  {
    equipments[j++] = node->data;

    node = node->next;
    i++;
  }

  *count = j;
  return equipments;
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
      equipment_list_reinsert(filtered, node->data);
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
      equipment_list_reinsert(filtered, node->data);
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
      equipment_list_reinsert(filtered, node->data);
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

bool equipment_exists_by_ip(hashmap_t *ip_index, const char *ip_address)
{
  equipment_node_t *node = (equipment_node_t *) hashmap_get(ip_index, ip_address);
  return (node != NULL) ? true : false;
}

bool equipment_exists_by_mac(hashmap_t *mac_index, const char *mac_address)
{
  equipment_node_t *node = (equipment_node_t *) hashmap_get(mac_index, mac_address);
  return (node != NULL) ? true : false;
}

int equipment_get_count(equipment_list_t *list)
{
  return list->count;
}

int equipment_get_number_status(equipment_list_t *list, equipment_status_t status)
{
  int i = 0;

  equipment_node_t *node = list->head;

  while (node != NULL)
  {
    if (node->data.status == status) i++;

    node = node->next;
  }

  return i;
}

void equipment_list_generate_report(equipment_list_t *list)
{
  FILE *file = fopen(filepath, "w");
  if (file == NULL) return;

  char date[DATE_MAX];
  format_current_date(date);

  fprintf(file, "==================================================\n");
  fprintf(file, "         NETPULSE NOC - NETWORK REPORT            \n");
  fprintf(file, "==================================================\n");
  fprintf(file, "Criado em: %s\n", date);
  fprintf(file, "Total de Equipamentos: %d\n", list->count);
  fprintf(file, "--------------------------------------------------\n\n");

  fprintf(file, "%s | %s | %s | %s\n", "ID", "NOME", "IP ADDRESS", "STATUS");
  fprintf(file, "--------------------------------------------------\n");

  equipment_node_t *node = list->head;

  while (node != NULL)
  {
    char id[ID_MAX];
    equipment_format_id(node->data.id, id);

    fprintf(file, "[%s] | %s | %s | %s\n", id, node->data.name, node->data.ip_address, equipment_status_to_string(node->data.status));

    node = node->next;
  }

  int operational = 0, failed = 0, maintenance = 0, disabled = 0;

  operational = equipment_get_number_status(list, STATUS_OPERATIONAL);
  failed = equipment_get_number_status(list, STATUS_FAILED);
  maintenance = equipment_get_number_status(list, STATUS_MAINTENANCE);
  disabled = equipment_get_number_status(list, STATUS_DISABLED);

  fprintf(file, "\n--------------------------------------------------\n");
  fprintf(file, "STATISTICS:\n");
  fprintf(file, "  [+] Operational: %d\n", operational);
  fprintf(file, "  [-] Failed: %d\n", failed);
  fprintf(file, "  [!] Maintenance:: %d\n", maintenance);
  fprintf(file, "  [*] Disabled:: %d\n", disabled);
  fprintf(file, "==================================================\n");

  fclose(file);
}

void equipment_format_id(int id, char *buffer)
{
  snprintf(buffer, ID_MAX, "EQ-%03d", id);
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

// Fast and Slow Pointer
static equipment_node_t *get_middle(equipment_node_t *node)
{
  equipment_node_t *slow = node;
  equipment_node_t *fast = node->next;

  while (fast != NULL && fast->next != NULL)
  {
    slow = slow->next;
    fast = fast->next->next;
  }

  return slow; // First element from the left side
}

static equipment_node_t *merge_sort(equipment_node_t *node, callback_fn compare)
{
  // Base case: list empty or already sorted
  if (node == NULL || node->next == NULL)
  {
    return node;
  }

  equipment_node_t *middle = get_middle(node);

  equipment_node_t *left = node;
  equipment_node_t *right = middle->next;

  middle->next = NULL;
  if (right != NULL) right->previous = NULL;

  left = merge_sort(left, compare);
  right = merge_sort(right, compare);

  return merge(left, right, compare);
}

static equipment_node_t *merge(equipment_node_t *left, equipment_node_t *right, callback_fn compare)
{
  if (left == NULL) return right;
  if (right == NULL) return left;

  equipment_node_t *node;

  if (compare(left, right) <= 0)
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

  node->previous = NULL;

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

