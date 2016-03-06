
/*
 * Note:
 * Compiling this with std=c11 will fail due to a redefinition error from alsa.
 * std=c99 will work, however.
 */

#include <alsa/asoundlib.h>
//#include <alsa/control.h>
#include <alsa/mixer.h>
#include <alloca.h>
#include <alsa/mixer.h>

#include <stdio.h>

#include "sound.h"

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

static long get_volume(void)
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

  long volume = 100 * (raw_volume - min) / (max - min);
  return volume;
}

void update_sound_info(SoundInfo *info)
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


  if (mixer_element != NULL) {
    info->muted = is_muted();
    info->volume = get_volume();
  } else {
    fprintf(stderr, "Mixer element is null.\n");
  }


  free_sound_resources(info);

}

void free_sound_resources(SoundInfo *info)
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

SoundInfo init_sound_info(void)
{
  SoundInfo info = {
    .muted = false,
    .volume = 0
  };
  return info;
}
