#include "app_screens.h"

#include "equipment_screen.h"
#include "connectivity_screen.h"
#include "sensor_screen.h"
#include "incident_screen.h"
#include "configuration_screen.h"

// TODO: Dashboard
static const screen_descriptor_t dashboard = {
  .number     = SCREEN_DASHBOARD, 
  .text       = "Dashboard", 
  .stack_name = "DASHBOARD", 
  .create     = NULL
};

const screen_descriptor_t *app_screens_get_descriptor(screen_number_t number)
{
  switch (number)
  {
    case SCREEN_DASHBOARD:     return &dashboard; // TODO
    case SCREEN_EQUIPMENT:     return equipment_screen_get_descriptor();
    case SCREEN_CONNECTIVITY:  return connectivity_screen_get_descriptor();
    case SCREEN_SENSOR:        return sensor_screen_get_descriptor();
    case SCREEN_INCIDENT:      return incident_screen_get_descriptor();
    case SCREEN_CONFIGURATION: return configuration_screen_get_descriptor();
    default:                   return NULL;
  }
}
