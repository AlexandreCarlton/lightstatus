#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  bool muted;
  uint_fast8_t volume;
} SoundInfo;

SoundInfo sound_init(void);

bool sound_update(SoundInfo * const);

bool sound_should_display(SoundInfo const * const);

void sound_print(FILE *, SoundInfo const * const);

void sound_free(SoundInfo * const);
