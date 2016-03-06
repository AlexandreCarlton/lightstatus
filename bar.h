#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "battery.h"
#include "bspwm.h"
#include "datetime.h"
#include "sound.h"
#include "packages.h"
#include "wifi.h"

typedef struct {
  //bool updated; // Would be nice if I could access it without expose it.
  BspwmInfo bspwm;
  WifiInfo wifi;
  SoundInfo sound;
  BatteryInfo battery;
  PackagesInfo packages;
  DateTimeInfo datetime;
} Bar;

void display_bar(FILE *, Bar const * const);

void free_bar_resources(Bar * const);

void print_bspwm_info(FILE *, BspwmInfo const * const);

void print_battery_info(FILE *, BatteryInfo const * const);

void print_datetime_info(FILE *, DateTimeInfo const * const);

void print_packages_info(FILE *, PackagesInfo const * const);

void print_sound_info(FILE *, SoundInfo const * const);

void print_wifi_info(FILE *, WifiInfo const * const);
