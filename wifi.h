#pragma once

#include <stdbool.h>

#define MAX_ESSID_LENGTH 50

typedef struct {
  unsigned int signal_strength;
  char name[MAX_ESSID_LENGTH];
} WifiInfo;

void update_wifi_info(WifiInfo *);

bool equal_wifi_info(WifiInfo *, WifiInfo *);

WifiInfo init_wifi_info(void);
