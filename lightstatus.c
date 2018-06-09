#define _XOPEN_SOURCE 700

#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "bar.h"
#include "battery.h"
#include "bspwm.h"
#include "sound.h"
#include "packages.h"
#include "wifi.h"

#include "setup_loop.h"


int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  Bar bar = bar_init();

  struct ev_loop *loop = EV_DEFAULT;

  add_callbacks(&bar, stdin, stdout);


  // Update what we can
  sound_update(&bar.sound);
  wifi_update(&bar.wifi);
  battery_update(&bar.battery);
  clock_update(&bar.clock);

  // Start the watchers
  start_watchers(loop);

  ev_run(EV_DEFAULT_ 0);
  ev_loop_destroy(loop);

  return 0;
}
