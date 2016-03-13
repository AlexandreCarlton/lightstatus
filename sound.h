#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool muted;
  uint_fast8_t volume;
} SoundInfo;

void update_sound_info(SoundInfo *);

void free_sound_resources(SoundInfo *);

SoundInfo init_sound_info(void);
