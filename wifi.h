#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_ESSID_LENGTH 50

typedef struct {
  uint_fast8_t signal_strength;
  char name[MAX_ESSID_LENGTH];
} WifiInfo;

WifiInfo wifi_init(void);

bool wifi_update(WifiInfo *);

bool wifi_should_display(WifiInfo const * const);

void wifi_print(FILE *, WifiInfo const * const info);

void wifi_free(WifiInfo * const);
