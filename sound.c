
/*
 * Note:
 * Compiling this with std=c11 will fail due to a redefinition error from alsa.
 * std=c99 will work, however.
 */

#define _XOPEN_SOURCE 700

#include <alsa/asoundlib.h>
//#include <alsa/control.h>
#include <alsa/mixer.h>
#include <alloca.h>
#include <alsa/mixer.h>

#include <stdio.h>

#include "sound.h"
#include "config.h"
#include "icons.h"

#define ICON_ARRAY_LENGTH(arr) ((sizeof(arr)) / (sizeof(char *)))

static const char card[] = "default";
static const char mixer_name[] = "Master";
static unsigned int mixer_index = 0;


// Global in case of emergency free.
static snd_mixer_t *handle = NULL;
static snd_mixer_elem_t *mixer_element = NULL;

// TODO
// Have an volume_init and a volume_close
// And make the three snd_..._t elements static and global
// That way it's easier to deal with.
// volume_close needs to call snd_mixer_close(handle)
//
// Use the callback functions given to lazily alert.


static bool is_muted(void)
{
  bool muted = false;
  if (snd_mixer_selem_has_playback_switch(mixer_element)) {
    int playback_switch;
    snd_mixer_selem_get_playback_switch(mixer_element, SND_MIXER_SCHN_FRONT_LEFT, &playback_switch);
    muted = playback_switch == 0;
  } else {
    fprintf(stderr, "Error: Could not find a mute switch.");
  }
  return muted;
}

static uint_fast8_t get_volume(void)
{
  long raw_volume = 0;
  long min = 0;
  long max = 100;
  snd_mixer_selem_get_playback_volume_range(mixer_element, &min, &max);
  // Could get the right one as well and average the two.
  if (snd_mixer_selem_get_playback_volume(mixer_element,
        SND_MIXER_SCHN_FRONT_LEFT,
        &raw_volume)) {
    fprintf(stderr, "Error: Unable to retrieve playback volume.\n");
  }

  uint_fast8_t volume = (uint_fast8_t) (100 * (raw_volume - min) / (max - min));
  return volume;
}

bool sound_update(SoundInfo * const info)
{

  if (snd_mixer_open(&handle, 0) < 0) {
    fprintf(stderr, "Error opening handle.\n");
  }
  if (snd_mixer_attach(handle, card)) {
    fprintf(stderr, "Error attaching handle to card.\n");
  }
  if (snd_mixer_selem_register(handle, NULL, NULL)) {
    fprintf(stderr, "Error registering handle.\n");
  }
  if (snd_mixer_load(handle) < 0) {
    fprintf(stderr, "Error loading handle.\n");
  }

  snd_mixer_selem_id_t *sid = NULL;
  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, mixer_index);
  snd_mixer_selem_id_set_name(sid, mixer_name);
  mixer_element = snd_mixer_find_selem(handle, sid); // NULL if not found


  /* if (snd_mixer_close(handle) < 0) { */
  /*   fprintf(stderr, "Error closing handle\n"); */
  /* } */


  bool updated = false;
  if (mixer_element != NULL) {
    SoundInfo old_info = {
      .muted = info->muted,
      .volume = info->volume
    };
    info->muted = is_muted();
    info->volume = get_volume();
    updated = (info->muted != old_info.muted) || (info->volume != old_info.volume);
  } else {
    fprintf(stderr, "Mixer element is null.\n");
  }


  sound_free(info);

  return updated;

}

void sound_free(SoundInfo * const info)
{
  (void) info;
  // Still have a memory leak somewhere.

  snd_config_update_free_global(); // Free the cache.

  /* if (sid != NULL) { */
  /*   snd_mixer_selem_id_free(sid); */
  /*   sid = NULL; */
  /* } */
  /* if (mixer_element != NULL) { */
  /*   snd_mixer_elem_free(mixer_element); */
  /*   mixer_element = NULL; */
  /* } */
  if (handle != NULL && snd_mixer_detach(handle, card) < 0) {
    fprintf(stderr, "Error detaching handle from the card.\n");
  }
  if (handle != NULL && snd_mixer_close(handle)) {
    fprintf(stderr, "Error closing handle.\n");
  }
  /* if (handle != NULL) { */
  /*   snd_mixer_free(handle); */
  /* } */
  handle = NULL;
}

SoundInfo sound_init(void)
{
  SoundInfo info = {
    .muted = false,
    .volume = 0
  };
  return info;
}

bool sound_should_display(SoundInfo const * const info)
{
  (void) info;
  return true;
}

void sound_print(FILE *file, SoundInfo const * const info)
{
  if (info->muted || info->volume == 0) {
    fprintf(file, "%s -", sound_muted_icon);
  } else {
    const char *sound_unmuted_icon = sound_unmuted_icons[0];
    unsigned long len_icons = ICON_ARRAY_LENGTH(sound_unmuted_icons);
    for (unsigned long i = 1; i <= len_icons; i++)
    {
      if (info->volume <= i * 100 / len_icons) {
        sound_unmuted_icon = sound_unmuted_icons[i - 1];
        break;
      }
    }
    fprintf(file, "%s %hhu", sound_unmuted_icon, info->volume);

  }

}
