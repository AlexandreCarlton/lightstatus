#pragma once

#include <stdbool.h>

typedef struct {
  bool muted;
  long volume;
} SoundInfo;

void update_sound_info(SoundInfo *);

void free_sound_resources(SoundInfo *);

SoundInfo init_sound_info(void);
