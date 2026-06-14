#include "app_data.h"

void app_data_init(app_data_t *data)
{
  equipment_list_init(&data->equipments);
  incident_queue_init(&data->incidents_pending);
  incident_list_init(&data->incidents_history);
  sensor_array_init(&data->sensors);
  technician_list_init(&data->technicians);

  hashmap_init(&data->id_index);
  hashmap_init(&data->ip_index);
  hashmap_init(&data->mac_index);
  hashmap_init(&data->username_index);

  // TODO: technician_t *current_user
}

void app_data_destroy(app_data_t *data)
{
  hashmap_destroy(&data->id_index);
  hashmap_destroy(&data->ip_index);
  hashmap_destroy(&data->mac_index);
  hashmap_destroy(&data->username_index);

  equipment_list_destroy(&data->equipments);
  incident_queue_destroy(&data->incidents_pending);
  incident_list_destroy(&data->incidents_history);
  sensor_array_destroy(&data->sensors);
  technician_list_destroy(&data->technicians);
  
  // TODO: technician_t *current_user
}
