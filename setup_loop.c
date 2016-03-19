#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "setup_loop.h"

#define BUFSIZE 1024

// TODO we could instead start prepare_display in each callback
//    (would it be necessary to check if had already been activated?)
// and prepare_display would then cancel itself on call.
// Pros:
//  - Not constantly calling prepare only to have it blank
//  - No need for global static variable
// Cons:
//  - More repetition?
static FILE* out_fp;

static ev_io bspwm_watcher;
static ev_timer wifi_timer;
static ev_timer battery_watcher;
static ev_timer sound_watcher;
static ev_timer packages_timer;
static ev_io packages_io_watcher;
static ev_periodic clock_watcher;
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
    bspwm_update(info, line);
  }
}

static void
wifi_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
  (void) loop;
  (void) revents;
  WifiInfo *info = (WifiInfo *) w->data;
  wifi_update(info);
}

static void
battery_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
  (void) loop;
  (void) revents;
  BatteryInfo *info = (BatteryInfo *)w->data;
  battery_update(info);
}

static void
sound_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
  (void) loop;
  (void) revents;
  SoundInfo *info = (SoundInfo *) w->data;
  sound_update(info);
}

static void
packages_io_cb(struct ev_loop *loop, ev_io *w, int revents)
{
  (void) revents;
  ev_io_stop(loop, w);

  FILE *packages_file = fdopen(w->fd, "r");
  PackagesInfo *info = (PackagesInfo *) w->data;
  packages_update(info, packages_file);
  pclose(packages_file);
  ev_timer_again(loop, &packages_timer);
}

static void
packages_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
  (void) revents;
  // We need to stop the timer in case our repeat interval is smaller than the
  // time taken for our command to yield results.
  ev_timer_stop(loop, w);
  FILE *packages_file = popen("checkupdates", "r");
  ev_io_set(&packages_io_watcher, fileno(packages_file), EV_READ);
  ev_io_start(loop, &packages_io_watcher);
}

static void
clock_cb(struct ev_loop *loop, ev_periodic *w, int revents)
{
  (void) loop;
  (void) revents;
  ClockInfo *info = (ClockInfo *) w->data;
  clock_update(info);
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
  bar_print(out_fp, bar);
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

  packages_io_watcher.data = &bar->packages;
  FILE *packages_file = get_packages_file();
  ev_io_init(&packages_io_watcher, packages_io_cb, fileno(packages_file), EV_READ);
  ev_init(&packages_timer, packages_cb);
  packages_timer.repeat = 5 * 60.;

  // include an offset so that we don't get the time before it changes.
  // TODO Maybe separate into date watcher and time watcher?
  clock_update(&bar->clock);
  clock_watcher.data = &bar->clock;
  ev_periodic_init(&clock_watcher, clock_cb, 1., 60., 0);

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
  ev_io_start(loop, &packages_io_watcher);
  ev_periodic_start(loop, &clock_watcher);
  ev_signal_start(loop, &signal_watcher);
  ev_prepare_start(loop, &prepare_display);

}

