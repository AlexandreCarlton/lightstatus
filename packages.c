#include <stdio.h>
#include <stdlib.h>

#include "packages.h"


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

void update_packages_info(PackagesInfo *info, FILE *packages_file)
{
  uint_fast16_t num_packages = count_lines(packages_file);
  info->to_update = num_packages;
}

PackagesInfo init_packages_info(void)
{
  PackagesInfo info = {
    .to_update = 0
  };
  return info;
}
