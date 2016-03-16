#include <stdlib.h>

#include "bar.h"
#include "config.h"
#include "icons.h"

#define ICON_ARRAY_LENGTH(arr) ((sizeof(arr)) / (sizeof(char *)))


void print_packages_info(FILE *file, PackagesInfo const * const info)
{
  fprintf(file, "%s %lu", packages_icon, info->to_update);
}

void print_datetime_info(FILE *file, DateTimeInfo const * const info)
{
  fprintf(file, "%s %s", time_icon, info->formatted);
}

void display_bar(FILE *file, Bar const * const bar)
{
  fprintf(file, "%%{l}");

  if (bspwm_should_display(&bar->bspwm)) {
    bspwm_print(file, &bar->bspwm);
  }

  fprintf(file, "%%{c}%%{r}");

  fprintf(file, "%%{F%s}%%{B%s}", color_sys_fg, color_sys_bg);

  if (wifi_should_display(&bar->wifi)) {
    wifi_print(file, &bar->wifi);
  }

  if (battery_should_display(&bar->battery)) {
    battery_print(file, &bar->battery);
    fputc(' ', file);
  }

  if (sound_should_display(&bar->sound)) {
    sound_print(file, &bar->sound);
  }

  fputc(' ', file);

  if (bar->packages.to_update > 0) {
    print_packages_info(file, &bar->packages);
    fputc(' ', file);
  }

  print_datetime_info(file, &bar->datetime);

  fprintf(file, "%%{B-}%%{F-}");

  fputs(" \n", file);

  fflush(file);
}

void free_bar_resources(Bar * const bar)
{
  bspwm_free(&bar->bspwm);
  battery_free(&bar->battery);
  sound_free(&bar->sound);
  wifi_free(&bar->wifi);
}
