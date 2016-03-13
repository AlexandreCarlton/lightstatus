#pragma once

#include <stdint.h>

typedef enum {
  DISCHARGING,
  CHARGING,
  FULL
} BatteryState;

typedef struct {
  BatteryState state;
  uint_fast8_t percentage;
} BatteryInfo;

void update_battery_info(BatteryInfo *);

BatteryInfo init_battery_info(void);
