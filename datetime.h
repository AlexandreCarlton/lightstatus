#pragma once

#define TIME_BUFFER_LENGTH 50

typedef struct {
  char formatted[TIME_BUFFER_LENGTH];
} DateTimeInfo;

void update_datetime_info(DateTimeInfo *);

void *update_datetime_context(void *);

DateTimeInfo init_datetime_info(void);
