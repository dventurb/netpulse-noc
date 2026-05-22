#include "utils.h"

#include "macros.h"
#include <time.h>

void get_datetime(time_t time, char *string)
{
  struct tm *tm = localtime(&time);
  strftime(string, DATETIME_MAX, "%d-%m-%Y %H:%M", tm);
}
