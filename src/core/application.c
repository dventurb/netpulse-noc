#include "application.h"

void application_init(application_t *application)
{
  equipment_list_init(&application->equipments);
  incident_queue_init(&application->incidents_pending);
  incident_list_init(&application->incidents_history);
  sensor_array_init(&application->sensors);
  technician_list_init(&application->technicians);

  hashmap_init(&application->id_index);
  hashmap_init(&application->ip_index);
  hashmap_init(&application->mac_index);
  hashmap_init(&application->username_index);

  // TODO: technician_t *current_user
}

void application_destroy(application_t *application)
{
  hashmap_destroy(&application->id_index);
  hashmap_destroy(&application->ip_index);
  hashmap_destroy(&application->mac_index);
  hashmap_destroy(&application->username_index);

  equipment_list_destroy(&application->equipments);
  incident_queue_destroy(&application->incidents_pending);
  incident_list_destroy(&application->incidents_history);
  sensor_array_destroy(&application->sensors);
  technician_list_destroy(&application->technicians);
  
  // TODO: technician_t *current_user
}
