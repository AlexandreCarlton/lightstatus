#pragma once

#include <stdint.h>
#include <stdio.h>

// Is it worth wrapping this in a struct?
// Yeah, probably just to future-proof it.
typedef struct {
  uint_fast16_t to_update;
} PackagesInfo;

void update_packages_info(PackagesInfo *, FILE *);

PackagesInfo init_packages_info(void);
