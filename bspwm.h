#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

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

bool bspwm_update(BspwmInfo * const, char *);

bool bspwm_should_display(BspwmInfo const * const);

void bspwm_free(BspwmInfo * const);

BspwmInfo bspwm_init(void);

void bspwm_print(FILE *, BspwmInfo const * const);
