#include <stdlib.h>

#include "bar.h"
#include "config.h"
#include "icons.h"

#define ICON_ARRAY_LENGTH(arr) ((sizeof(arr)) / (sizeof(char *)))


void print_wifi_info(FILE *file, WifiInfo const * const info)
{
  const char *wifi_connected_icon = wifi_connected_icons[0];
  unsigned long len_icons = sizeof(wifi_connected_icons) / sizeof(char *);
  for (unsigned long i = 1; i <= len_icons; i++)
  {
    if (info->signal_strength <= i * 100 / len_icons) {
      wifi_connected_icon = wifi_connected_icons[i - 1];
      break;
    }
  }
  if (info->signal_strength > 0) {
    fprintf(file, "%s %s", wifi_connected_icon, info->name);
  } else {
    fprintf(file, "%s %s", wifi_disconnected_icon, "N/A");
  }
}

void print_sound_info(FILE *file, SoundInfo const * const info)
{
  if (info->muted || info->volume == 0) {
    fprintf(file, "%s -", sound_muted_icon);
  } else {
    const char *sound_unmuted_icon = sound_unmuted_icons[0];
    unsigned long len_icons = ICON_ARRAY_LENGTH(sound_unmuted_icons);
    for (unsigned long i = 1; i <= len_icons; i++)
    {
      if (info->volume <= i * 100 / len_icons) {
        sound_unmuted_icon = sound_unmuted_icons[i - 1];
        break;
      }
    }
    fprintf(file, "%s %hhu", sound_unmuted_icon, info->volume);

  }
}

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

  if (bar->wifi.signal_strength > 0) {
    print_wifi_info(file, &bar->wifi);
    fputc(' ', file);
  }


  if (battery_should_display(&bar->battery)) {
    battery_print(file, &bar->battery);
    fputc(' ', file);
  }


  print_sound_info(file, &bar->sound);

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
  free_sound_resources(&bar->sound);
}
