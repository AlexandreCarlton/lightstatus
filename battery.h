#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
  DISCHARGING,
  CHARGING,
  FULL
} BatteryState;

typedef struct {
  BatteryState state;
  uint_fast8_t percentage;
} BatteryInfo;

BatteryInfo battery_init(void);

bool battery_update(BatteryInfo * const);

bool battery_should_display(BatteryInfo const * const);

void battery_print(FILE *, BatteryInfo const * const);

void battery_free(BatteryInfo * const);

