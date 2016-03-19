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

  FILE *output_file = stdout;

  int opt;
  while ((opt = getopt(argc, argv, ":o:")) != -1){
    switch (opt) {
      case 'o':
        output_file = fopen(optarg, "w");
    }
  }

  Bar bar = bar_init();

  struct ev_loop *loop = EV_DEFAULT;

  add_callbacks(&bar, STDIN_FILENO, fileno(output_file));


  // Update what we can
  sound_update(&bar.sound);
  wifi_update(&bar.wifi);
  battery_update(&bar.battery);
  clock_update(&bar.clock);

  // Start the watchers
  start_watchers(loop);

  ev_run(EV_DEFAULT_ 0);
  ev_loop_destroy(loop);

  //close(STDIN_FILENO);
  //close(STDOUT_FILENO);

  if (output_file != stdout && output_file != NULL) {
    fclose(output_file);
  }

  return 0;
}
