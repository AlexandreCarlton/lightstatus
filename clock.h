#pragma once

#include <stdbool.h>
#include <stdio.h>


#define TIME_BUFFER_LENGTH 10

typedef struct {
  char formatted[TIME_BUFFER_LENGTH];
} ClockInfo;

ClockInfo clock_init(void);

bool clock_update(ClockInfo * const);

bool clock_should_display(ClockInfo const * const);

void clock_print(FILE *, ClockInfo const * const);

void clock_free(ClockInfo * const);
