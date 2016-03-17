#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Is it worth wrapping this in a struct?
// Yeah, probably just to future-proof it.
typedef struct {
  uint_fast16_t to_update;
} PackagesInfo;

FILE *get_packages_file(void);

PackagesInfo packages_init(void);

bool packages_update(PackagesInfo *, FILE *);

bool packages_should_display(PackagesInfo const * const);

void packages_print(FILE *, PackagesInfo const * const);

void packages_free(PackagesInfo * const);
