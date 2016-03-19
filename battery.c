
// We can use udevadm or acpi_listen (using acpid) to listen for battery events
// so instead of polling ourselves, we look to udevadm to do it for us.
//
// acpid starts up polkit
//
// udevadm monitor --subsystem-match=power_supply --kernel
//  - every line corresponds to change in battery
//  - don't need to read it, just activate the callback, and set updated = true.
//  - So this file doesn't change.
//
//  at the end of the day, either of these solutions spawns another process...
//  It's just easier to poll a file every second.



#include "battery.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "config.h"


#define BATTERY_FOLDER "/sys/class/power_supply/BAT0"
// Possible values of BATTERY_STATUS
#define BATTERY_CHARGING "Charging"
#define BATTERY_DISCHARGING "Discharging"
#define BATTERY_FULL "Full"
// The longest status is "Discharging", which is 11 characters long.
#define BATTERY_STATUS_LENGTH 12

// Maybe return the error code and modify a state instead?
static BatteryState get_battery_state(void)
{
  char status[BATTERY_STATUS_LENGTH];
  BatteryState state = DISCHARGING;
  FILE *file_charging = fopen(BATTERY_FOLDER "/status", "r");

  if (file_charging) {
    fscanf(file_charging, "%s\n", status);
    fclose(file_charging);
    if (strcmp(status, BATTERY_CHARGING) == 0) {
      state = CHARGING;
    } else if (strcmp(status, BATTERY_FULL) == 0) {
      state = FULL;
    } else if (strcmp(status, BATTERY_DISCHARGING) == 0) {
      state = DISCHARGING;
    }
  }
  return state;
}

// We could return -1 on failure.
static uint_fast8_t get_battery_remaining(void)
{
  uint_fast8_t percentage_remaining = 0;
  unsigned long charge_now = 0;
  unsigned long charge_full = 1;

  FILE *file_now = fopen(BATTERY_FOLDER "/charge_now", "r");
  FILE *file_full = fopen(BATTERY_FOLDER "/charge_full", "r");

  if (file_now && file_full) {
    fscanf(file_now, "%lu\n", &charge_now);
    fscanf(file_full, "%lu\n", &charge_full);
    percentage_remaining = (uint_fast8_t) (100 * charge_now / charge_full);
  }

  if (file_now) fclose(file_now);
  if (file_full) fclose(file_full);

  return percentage_remaining;
}


bool battery_should_display(BatteryInfo const * const info)
{
  return info->state != FULL;
}

void battery_print(FILE *file, BatteryInfo const * const info)
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

bool battery_update(BatteryInfo * const info)
{
  BatteryInfo old_info = {
    .state = info->state,
    .percentage = info->percentage
  };
  info->state = get_battery_state();
  info->percentage = get_battery_remaining();

  return (info->state != old_info.state)
    || (info->percentage != old_info.percentage);
}

BatteryInfo battery_init(void)
{
  BatteryInfo info = {
    .state = DISCHARGING,
    .percentage = 0
  };
  return info;
}

void battery_free(BatteryInfo * const info)
{
  (void) info;
}
