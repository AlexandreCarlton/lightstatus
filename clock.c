
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include "clock.h"
#include "icons.h"
#include "config.h"

bool clock_update(ClockInfo * const info)
{
  time_t timer;
  time(&timer);

  struct tm* tm_info = localtime(&timer);

  // %a %d %h for date.
  strftime(info->formatted, TIME_BUFFER_LENGTH, "%H:%M", tm_info);
  return true;
}

ClockInfo clock_init(void)
{
  ClockInfo info = {
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
  fprintf(file, "%s %s", clock_icon, info->formatted);
}

void clock_free(ClockInfo * const info)
{
  (void) info;
}
