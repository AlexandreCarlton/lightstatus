#pragma once

#include "bar.h"

#include <ev.h>

void add_callbacks(Bar * const bar, int, int);

void start_watchers(struct ev_loop *loop);
