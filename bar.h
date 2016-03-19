#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "battery.h"
#include "bspwm.h"
#include "clock.h"
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
  ClockInfo clock;
} Bar;

void display_bar(FILE *, Bar const * const);

void free_bar_resources(Bar * const);
