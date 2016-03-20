#pragma once

#include "bar.h"

#include <stdio.h>
#include <ev.h>

void add_callbacks(Bar * const bar, FILE *, FILE *);

void start_watchers(struct ev_loop *loop);
