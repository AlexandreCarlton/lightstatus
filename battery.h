#pragma once

typedef enum {
  DISCHARGING,
  CHARGING,
  FULL
} BatteryState;

typedef struct {
  BatteryState state;
  unsigned int percentage;
} BatteryInfo;

void update_battery_info(BatteryInfo *);

BatteryInfo init_battery_info(void);
