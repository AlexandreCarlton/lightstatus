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

  Bar bar = {
    .bspwm = bspwm_init(),
    .wifi = init_wifi_info(),
    .battery = init_battery_info(),
    .sound = init_sound_info(),
    .packages = init_packages_info(),
    .datetime = init_datetime_info()
  };

  struct ev_loop *loop = EV_DEFAULT;

  FILE *out_fp = fopen("/run/user/1000/lemonbar-fifo", "w");
  /* out_fp = stdout; */

  // TODO If feeling fancy, could also wait until out_fp is ready before writing to it.

  add_callbacks(&bar, STDIN_FILENO, fileno(out_fp));


  // Update what we can
  update_sound_info(&bar.sound);
  update_wifi_info(&bar.wifi);
  update_battery_info(&bar.battery);
  update_datetime_info(&bar.datetime);

  // Start the watchers
  start_watchers(loop);

  ev_run(EV_DEFAULT_ 0);
  ev_loop_destroy(loop);

  //close(STDIN_FILENO);
  //close(STDOUT_FILENO);

  //fclose(out_fp);

  return 0;
}
