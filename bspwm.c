
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bspwm.h"
#include "config.h" // config.h should contain the icons and colors.
#include "icons.h"

#define MAX_LINE 128
#define ICON_ARRAY_LENGTH(arr) ((sizeof(arr)) / (sizeof(char *)))

static char delimiter = ':';

static size_t get_num_desktops(char *line)
{

  size_t count = 0;
  for (char *p = line; *p != '\0'; p++) {
     count += (*p == delimiter);
  }
  return count - 1;
}

static void update_num_desktops(BspwmInfo * const info, char *line)
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

bool bspwm_update(BspwmInfo * const info, char * const line)
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
  return true;
}

bool bspwm_should_display(BspwmInfo const * const info)
{
  return info->desktops != NULL;
}

void bspwm_free(BspwmInfo * const info)
{
  if (info->desktops) {
    free(info->desktops);
    info->desktops = NULL;
  }
}

BspwmInfo bspwm_init(void)
{
  BspwmInfo info = {
   .desktops = NULL,
   .layout = TILED,
   .num_desktops = 0
  };
  return info;
}

void bspwm_print(FILE *file, BspwmInfo const * const info)
{
  for (unsigned int i = 0; i < info->num_desktops; i++) {
    BspwmDesktop desktop = info->desktops[i];
    const char *fg = color_free_fg;
    const char *bg = color_free_bg;
    if (desktop.urgent && desktop.focused) {
      fg = color_focused_urgent_fg;
      bg = color_focused_urgent_bg;
    } else if (desktop.urgent && !desktop.focused) {
      fg = color_urgent_fg;
      bg = color_urgent_bg;
    } else if (desktop.occupied && desktop.focused) {
      fg = color_focused_occupied_fg;
      bg = color_focused_occupied_bg;
    } else if (desktop.occupied && !desktop.focused) {
      fg = color_occupied_fg;
      bg = color_occupied_bg;
    } else if (desktop.focused) {
      fg = color_focused_free_fg;
      bg = color_focused_free_bg;
    }
    fprintf(file, "%%{F%s}%%{B%s} %s %%{B-}%%{F-}", fg, bg, desktop.name);
  }

  char layout;
  switch (info->layout) {
    case TILED:
      layout = 'T';
      break;
    case MONOCLE:
      layout = 'M';
      break;
    default:
      layout = '_';
      break;
  }
  fprintf(file, "%%{F%s}%%{B%s} %c %%{B-}%%{F-}", color_state_fg, color_state_bg, layout);
}
