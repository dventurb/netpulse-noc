#include "app.h"
#include "persistence.h"

int main(int argc, char **argv)
{
  app_t app;
  app_init(&app);

  load_equipments(&app.data);
  load_incidents(&app.data.incidents_pending, &app.data.incidents_history);

  int status = app_launcher(&app, argc, argv);

  app_destroy(&app);

  return status;
}
