#include <stdlib.h>

#include "bar.h"
#include "config.h"
#include "icons.h"

#define ICON_ARRAY_LENGTH(arr) ((sizeof(arr)) / (sizeof(char *)))

void print_battery_info(FILE *file, BatteryInfo const * const info)
{
  const char *battery_discharging_icon = battery_discharging_icons[0];
  unsigned long len_icons = sizeof(battery_discharging_icons) / sizeof(char *);

  for (unsigned long i = 1; i <= len_icons; i++)
  {
    if (info->percentage <= i * 100 / len_icons) {
      battery_discharging_icon = battery_discharging_icons[i - 1];
      break;
    }
  }

  switch (info->state) {
    case FULL:
      fprintf(file, "%s 100", battery_full_icon);
      break;
    case CHARGING:
      fprintf(file, "%s %u", battery_charging_icon, info->percentage);
      break;
    case DISCHARGING:
      fprintf(file, "%s %u", battery_discharging_icon, info->percentage);
      break;
  }
}

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

void print_bspwm_info(FILE *file, BspwmInfo const * const info)
{
  for (unsigned int i = 0; i < info->num_desktops; i++) {
    BspwmDesktop desktop = info->desktops[i];
    const char *fg = color_free_fg;
    const char *bg = color_free_bg;
    if (desktop.urgent && desktop.focused) {
      fg = color_focused_urgent_fg;
      bg = color_focused_urgent_bg;
    } else if (desktop.urgent && !desktop.focused) {
      fg = color_urgent_fg;
      bg = color_urgent_bg;
    } else if (desktop.occupied && desktop.focused) {
      fg = color_focused_occupied_fg;
      bg = color_focused_occupied_bg;
    } else if (desktop.occupied && !desktop.focused) {
      fg = color_occupied_fg;
      bg = color_occupied_bg;
    } else if (desktop.focused) {
      fg = color_focused_free_fg;
      bg = color_focused_free_bg;
    }
    fprintf(file, "%%{F%s}%%{B%s} %s %%{B-}%%{F-}", fg, bg, desktop.name);
  }

  char layout;
  switch (info->layout) {
    case TILED:
      layout = 'T';
      break;
    case MONOCLE:
      layout = 'M';
      break;
    default:
      layout = '_';
      break;
  }
  fprintf(file, "%%{F%s}%%{B%s} %c %%{B-}%%{F-}", color_state_fg, color_state_bg, layout);
}

void display_bar(FILE *file, Bar const * const bar)
{
  fprintf(file, "%%{l}");

  print_bspwm_info(file, &bar->bspwm);

  fprintf(file, "%%{c}%%{r}");

  fprintf(file, "%%{F%s}%%{B%s}", color_sys_fg, color_sys_bg);

  if (bar->wifi.signal_strength > 0) {
    print_wifi_info(file, &bar->wifi);
    fputc(' ', file);
  }


  if (bar->battery.state != FULL) {
    print_battery_info(file, &bar->battery);
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
  free_bspwm_resources(&bar->bspwm);
  free_sound_resources(&bar->sound);
}
