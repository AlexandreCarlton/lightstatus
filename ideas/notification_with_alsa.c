#define _XOPEN_SOURCE 700
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <alloca.h>
#include <ev.h>
#include <stdio.h>

// TODO:
// https://github.com/AndrewCooper/auduv/blob/master/auduv.c for more examples

ev_io sound_watcher;
struct pollfd *pfds;
static snd_mixer_t *mixer = NULL;
static snd_mixer_elem_t *mixer_element = NULL;

static void
sound_cb(EV_P_ ev_io *w, int revents)
{
  (void) loop;
  (void) w;
  (void) revents;
  int playback_switch;
  /* unsigned short revents_; */
  /* snd_mixer_poll_descriptors_revents(mixer, pfds, 1, &revents_); */
  snd_mixer_handle_events(mixer); // necessary so we make the fd unreadable again.

  snd_mixer_selem_get_playback_switch(mixer_element, SND_MIXER_SCHN_FRONT_LEFT, &playback_switch);
  printf("Muted: %s\n", playback_switch == 0 ? "True" : "False");

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

  unsigned int volume = (unsigned int) (100 * (raw_volume - min) / (max - min));
  printf("Volume: %u\n", volume);
}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  unsigned short revents;

  snd_mixer_open(&mixer, 0);
  snd_mixer_attach(mixer, "default");
  snd_mixer_selem_register(mixer, NULL, NULL);
  snd_mixer_load(mixer);

  snd_mixer_selem_id_t *sid = NULL;
  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, "Master");
  mixer_element = snd_mixer_find_selem(mixer, sid);

  unsigned int count = snd_mixer_poll_descriptors_count(mixer);
  printf("Count = %u\n", count);
  pfds = malloc(count * sizeof(struct pollfd));
  snd_mixer_poll_descriptors(mixer, pfds, count);

  struct ev_loop *loop = EV_DEFAULT;
  for (unsigned int i = 0; i < count; i++) {
    printf("Initialising struct pollfd.\n");
    ev_io_init(&sound_watcher, sound_cb, pfds[i].fd, EV_READ);
  }
  ev_io_start(loop, &sound_watcher);

  ev_run(loop, 0);

  return 0;
}
