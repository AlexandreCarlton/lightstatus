#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>

#include "packages.h"
#include "config.h"
#include "icons.h"

static uint_fast16_t count_lines(FILE *fp)
{
  uint_fast16_t lines = 0;
  int c;
  while (!feof(fp)) {
    c = fgetc(fp);
    if (c == '\n') {
      lines++;
    }
  }
  return lines;
}

FILE *get_packages_file(void)
{
  return popen("checkupdates", "r");
}

PackagesInfo packages_init(void)
{
  PackagesInfo info = {
    .to_update = 0
  };
  return info;
}

bool packages_update(PackagesInfo * const info, FILE *packages_file)
{
  PackagesInfo old_info = { .to_update = info->to_update };
  uint_fast16_t num_packages = count_lines(packages_file);
  info->to_update = num_packages;
  return info->to_update != old_info.to_update;
}

bool packages_should_display(PackagesInfo const * const info)
{
  return info->to_update > 0;
}

void packages_print(FILE *file, PackagesInfo const * const info)
{
  fprintf(file, "%s %lu", packages_icon, info->to_update);
}

void packages_free(PackagesInfo * const info)
{
  (void) info;
}
