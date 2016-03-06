
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bspwm.h"

#define MAX_LINE 128

static char delimiter = ':';

static size_t get_num_desktops(char *line) {

  size_t count = 0;
  for (char *p = line; *p != '\0'; p++) {
     count += (*p == delimiter);
  }
  return count - 1;
}

static void update_num_desktops(BspwmInfo *info, char *line)
{
  size_t num_previous_desktops = info->num_desktops;
  info->num_desktops = get_num_desktops(line);

  if (info->num_desktops != num_previous_desktops) {

    /* info->desktops = (BspwmDesktop *) realloc( */
    /*     info->desktops, */
    /*     sizeof(BspwmDesktop) * info->num_desktops */
    /* ); */
    free(info->desktops);
    info->desktops = (BspwmDesktop *) malloc(sizeof(BspwmDesktop) * info->num_desktops);
  }
}

void update_bspwm_info(BspwmInfo *info, char *line)
{
  update_num_desktops(info, line);

  BspwmDesktop *current_desktop = info->desktops;

  // Skip first token, don't need it.
  for (char *token = strtok(line, ":");
       token != NULL;
       token = strtok(NULL, ":")) {

  // Really, should be looking at panel source, converting that.
    // Caps: occupied
    // f/F: free
    // o/O: occupied
    // u/U: urgent (?)
    // TODO: Make this shorter.
    switch (*token) {
      case 'W':
        // Not really needed here.
        break;
      case 'L':
        switch(token[1]) {
          case 'T':
            info->layout = TILED;
            break;
          case 'M':
            info->layout = MONOCLE;
            break;
        }
        break;
      case 'O':
        current_desktop->focused = true;
        current_desktop->occupied = true;
        current_desktop->urgent = false;
        strncpy(current_desktop->name, token + 1, strlen(token + 1) + 1);
        current_desktop++;
        break;
      case 'o':
        current_desktop->focused = false;
        current_desktop->occupied = true;
        current_desktop->urgent = false;
        strncpy(current_desktop->name, token + 1, strlen(token + 1) + 1);
        current_desktop++;
        break;
      case 'F':
        current_desktop->focused = true;
        current_desktop->occupied = false;
        current_desktop->urgent = false;
        strncpy(current_desktop->name, token + 1, strlen(token + 1) + 1);
        current_desktop++;
        break;
      case 'f':
        current_desktop->focused = false;
        current_desktop->occupied = false;
        current_desktop->urgent = false;
        strncpy(current_desktop->name, token + 1, strlen(token + 1) + 1);
        current_desktop++;
        break;
      case 'U':
        current_desktop->focused = true;
        current_desktop->occupied = true;
        current_desktop->urgent = true;
        strncpy(current_desktop->name, token + 1, strlen(token + 1) + 1);
        current_desktop++;
        break;
      case 'u':
        current_desktop->focused = false;
        current_desktop->occupied = true;
        current_desktop->urgent = true;
        strncpy(current_desktop->name, token + 1, strlen(token + 1) + 1);
        current_desktop++;
        break;
    }
  }
}

void free_bspwm_resources(BspwmInfo *info)
{
  if (info->desktops) {
    free(info->desktops);
    info->desktops = NULL;
  }
}

BspwmInfo init_bspwm_info(void)
{
  BspwmInfo info = {
   .desktops = NULL,
   .layout = TILED,
   .num_desktops = 0
  };
  return info;
}
