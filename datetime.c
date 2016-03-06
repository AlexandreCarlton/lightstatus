
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include "datetime.h"

void update_datetime_info(DateTimeInfo *info)
{
  time_t timer;
  time(&timer);

  struct tm* tm_info = localtime(&timer);

  strftime(info->formatted, TIME_BUFFER_LENGTH, "%a %d %h %H:%M", tm_info);
}

DateTimeInfo init_datetime_info(void)
{
  DateTimeInfo info = {
    .formatted = ""
  };
  return info;
}
