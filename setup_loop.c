#include <stdlib.h>

#include "setup_loop.h"

#define BUFSIZE 1024

// TODO we could instead start prepare_display in each callback,
// and prepare_display would then cancel itself on call.
// Pros:
//  - Not constantly calling prepare only to have it blank
//  - No need for global static variable
// Cons:
//  - More repetition?
static bool updated = false;
static FILE* out_fp;

static ev_io bspwm_watcher;
static ev_timer wifi_timer;
static ev_timer battery_watcher;
static ev_timer sound_watcher;
static ev_timer packages_watcher;
static ev_periodic datetime_watcher;
static ev_signal signal_watcher;
static ev_prepare prepare_display;

static void
bspwm_cb(struct ev_loop *loop, ev_io *w, int revents)
{
  (void) revents;
  (void) loop;

  //TODO: Find a way to close the fd? Can't always assume it'll be stdin.
  // Given we set it in main(), we can close it in main as well.
  FILE *file = fdopen(w->fd, "r");
  char line[BUFSIZE];
  BspwmInfo *info = (BspwmInfo *) w->data;
  if (fgets(line, BUFSIZE, file) != NULL)
  {
    update_bspwm_info(info, line);
    updated = true;
  }
}

static void
wifi_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
  (void) revents;
  WifiInfo *info = (WifiInfo *) w->data;
  WifiInfo old_info;
  memcpy(&old_info, info, sizeof(WifiInfo));
  update_wifi_info(info);
  updated = !equal_wifi_info(info, &old_info);
  ev_timer_again(loop, w);
}

static void
battery_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
  (void) revents;
  (void) loop;
  BatteryInfo *info = (BatteryInfo *)w->data;
  BatteryInfo old_info = {
    .state = info->state,
    .percentage = info->percentage
  };

  update_battery_info(info);
  if (info->state != old_info.state
      || info->percentage != old_info.percentage) {
    updated = true;
  }
}

static void
sound_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
  (void) loop;
  (void) revents;

  SoundInfo *info = (SoundInfo *) w->data;
  SoundInfo old_info = {
    .muted = info->muted,
    .volume = info->volume
  };
  update_sound_info(info);
  if (old_info.muted != info->muted
      || old_info.volume != info->volume) {
    updated = true;
  }
}

static void
packages_io_cb(struct ev_loop *loop, ev_io *w, int revents)
{
  (void) revents;
  FILE *packages_file = fdopen(w->fd, "r");
  PackagesInfo *info = (PackagesInfo *) w->data;
  PackagesInfo old_info = {
    .to_update = info->to_update
  };
  update_packages_info(info, packages_file);
  pclose(packages_file);
  if (old_info.to_update != info->to_update) {
    updated = true;
  }

  // We're done here, we'll let the timer kick off things again.
  ev_io_stop(loop, w);
  free(w);
}

// This is called every X seconds, kicks off pacman watcher.
// TODO: have pacman_io watcher kick off the timer, in case checkupdates
// takes longer than the interval specified.
static void
packages_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
  (void) loop;
  (void) revents;
  //Now we activate it,
  FILE *packages_file;
  PackagesInfo *info = (PackagesInfo *)w->data;
  packages_file = popen("checkupdates", "r");
  int packages_descriptor = fileno(packages_file);

  ev_io *pacman_watcher = (ev_io *) malloc(sizeof(ev_io));
  pacman_watcher->data = (void *) info;

  // Bonus: Find way to make sure we don't have one running.
  ev_io_init(pacman_watcher, packages_io_cb, packages_descriptor, EV_READ);
  ev_io_start(loop, pacman_watcher);
  ev_timer_again(loop, w);
}

static void
datetime_cb(struct ev_loop *loop, ev_periodic *w, int revents)
{
  (void) loop;
  (void) revents;
  DateTimeInfo *info = (DateTimeInfo *) w->data;
  update_datetime_info(info);
  updated = true;
}

static void
shutdown_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
  (void) revents;
  fprintf(stderr, "Caught signal, shutting down bar...\n");
  Bar *bar = (Bar *)w->data;
  ev_unloop(loop, EVUNLOOP_ALL);
  ev_break(loop, EVBREAK_ALL);

  // Consider putting this in the cleanup watcher.
  //close(0); // close stdin.
  free_bar_resources(bar);
}

static  void
display_cb(struct ev_loop *loop, ev_prepare *w, int revents)
{
  (void) revents;
  (void) loop;
  Bar const * const bar = (Bar *) w->data;
  // Try not checking for updated, see what happens.
  if (updated) {
    display_bar(out_fp, bar);
    updated = false;
  }

}

// TODO: How to close off the things?
// Pass them through?
void add_callbacks(Bar * const bar, int in_fd, int out_fd)
{
  bspwm_watcher.data = &bar->bspwm;
  ev_io_init(&bspwm_watcher, bspwm_cb, in_fd, EV_READ);

  wifi_timer.data = &bar->wifi;
  ev_init(&wifi_timer, wifi_cb);
  wifi_timer.repeat = 3.;

  battery_watcher.data = &bar->battery;
  ev_init(&battery_watcher, battery_cb);
  battery_watcher.repeat = 1.;

  sound_watcher.data = &bar->sound;
  ev_init(&sound_watcher, sound_cb);
  sound_watcher.repeat = 1.;

  // Slight problem: This takes too long and blocks.
  // Idea: We popen it, get the file descriptor and place a io watcher on it
  // When readable, we get the packages, close the file, open a new one, and attach the current watcher again.
  // Note that we need to use fcntl with O_NONBLOCK so it doesn't block.
  // ... On second thought, is having non-blocking even necessary?
  // Yeah, probably; file pointers are buffered while file descriptors aren't;
  // in any case, we need to make it a descriptor.
  //
  // Also need to take into account that we need to cycle this.
  // So we'd ned to make a timer, and when we activate that timer
  // We popen, get the fd, and create an io watcher on that fd for READ.
  // When it's ready, we call update_package_info
  //  Then stop the watcher (free if malloc'd, have a suspicion we'd need to)
  // Then we add a timer again on the loop.
  //    Will we have a scoping problem? Might need to malloc it when stopping it.

  packages_watcher.data = &bar->packages;
  ev_init(&packages_watcher, packages_cb);
  packages_watcher.repeat = 5 * 60.;

  // include an offset so that we don't get the time before it changes.
  // TODO Maybe separate into date watcher and time watcher?
  update_datetime_info(&bar->datetime);
  datetime_watcher.data = &bar->datetime;
  ev_periodic_init(&datetime_watcher, datetime_cb, 1., 60., 0);

  // Easy way to shut down.
  signal_watcher.data = bar;
  ev_signal_init(&signal_watcher, shutdown_cb, SIGINT);

  // An ev prepare runs before ev_run collects events;
  // in other words, it's (almost) like it's after all the events that have been called.
  // So we add an update

  out_fp = fdopen(out_fd, "w");
  prepare_display.data = bar;
  ev_prepare_init(&prepare_display, display_cb);

}

void start_watchers(struct ev_loop *loop)
{
  ev_io_start(loop, &bspwm_watcher);
  ev_timer_again(loop, &wifi_timer);
  ev_timer_again(loop, &battery_watcher);
  ev_timer_again(loop, &sound_watcher);
  ev_timer_again(loop, &packages_watcher);
  // TODO Make separate watchers for time and date
  ev_periodic_start(loop, &datetime_watcher);
  ev_signal_start(loop, &signal_watcher);
  ev_prepare_start(EV_DEFAULT_ &prepare_display);

}

void free_loop_resources(void)
{

}
