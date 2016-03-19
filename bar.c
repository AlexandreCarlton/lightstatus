#include <stdlib.h>

#include "bar.h"
#include "config.h"

#define ICON_ARRAY_LENGTH(arr) ((sizeof(arr)) / (sizeof(char *)))

Bar bar_init(void)
{
  Bar bar = {
    .battery = battery_init(),
    .bspwm = bspwm_init(),
    .clock = clock_init(),
    .packages = packages_init(),
    .sound = sound_init(),
    .wifi = wifi_init()
  };
  return bar;
}

void bar_print(FILE *file, Bar const * const bar)
{
  fprintf(file, "%%{l}");

  if (bspwm_should_display(&bar->bspwm)) {
    bspwm_print(file, &bar->bspwm);
  }

  fprintf(file, "%%{c}%%{r}");

  fprintf(file, "%%{F%s}%%{B%s}", color_sys_fg, color_sys_bg);

  if (wifi_should_display(&bar->wifi)) {
    wifi_print(file, &bar->wifi);
    fputc(' ', file);
  }

  if (battery_should_display(&bar->battery)) {
    battery_print(file, &bar->battery);
    fputc(' ', file);
  }

  if (sound_should_display(&bar->sound)) {
    sound_print(file, &bar->sound);
    fputc(' ', file);
  }

  if (packages_should_display(&bar->packages)) {
    packages_print(file, &bar->packages);
    fputc(' ', file);
  }

  if (clock_should_display(&bar->clock)) {
    clock_print(file, &bar->clock);
    fputc(' ', file);
  }

  fputs("%{B-}%{F-}", file);

  fputc('\n', file);

  fflush(file);
}

void bar_free(Bar * const bar)
{
  battery_free(&bar->battery);
  bspwm_free(&bar->bspwm);
  clock_free(&bar->clock);
  packages_free(&bar->packages);
  sound_free(&bar->sound);
  wifi_free(&bar->wifi);
}
