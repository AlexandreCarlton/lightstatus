
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include "clock.h"
#include "config.h"

bool clock_update(ClockInfo * const info)
{
  time_t timer;
  time(&timer);
  struct tm* tm_info = localtime(&timer);
  info->hours = (uint_fast8_t) tm_info->tm_hour;
  info->minutes = (uint_fast8_t) tm_info->tm_min;
  return true;
}

ClockInfo clock_init(void)
{
  ClockInfo info = {
    .hours = 0,
    .minutes = 0,
    .formatted = ""
  };
  return info;
}

bool clock_should_display(ClockInfo const * const info)
{
  (void) info;
  return true;
}

void clock_print(FILE * file, ClockInfo const * const info)
{
  fprintf(file, "%s %02d:%02d", clock_icon, info->hours, info->minutes);
}

void clock_free(ClockInfo * const info)
{
  (void) info;
}
