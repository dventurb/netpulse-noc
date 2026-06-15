#include "app.h"
#include "persistence.h"
#include <sodium.h>

int main(int argc, char **argv)
{
  if (sodium_init() < 0) return 1;

  app_t app;
  app_init(&app);

  load_technicians(&app.data);
  load_equipments(&app.data);
  load_incidents(&app.data.incidents_pending, &app.data.incidents_history);

  int status = app_launcher(&app, argc, argv);

  app_destroy(&app);

  return status;
}
