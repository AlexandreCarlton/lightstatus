#pragma once

#include <stdbool.h>
#include <stddef.h>

// Look at subscribe.c for more info
typedef struct {
  bool focused;
  bool occupied;
  bool urgent;
  char name[20];

} BspwmDesktop;

typedef enum {
  MONOCLE,
  TILED,
} BspwmLayout;

typedef struct {
  BspwmDesktop *desktops;
  size_t num_desktops;
  BspwmLayout layout;
} BspwmInfo;

void update_bspwm_info(BspwmInfo *, char *);

void free_bspwm_resources(BspwmInfo *);

BspwmInfo init_bspwm_info(void);

void print_bspwm(BspwmInfo);
