#ifndef _PROBES_H
#define _PROBES_H

#include "types.h"

struct fsent;

extern unsigned short fsent_deleted_semaphore;

void fsent_deleted_probe(const struct fsent * restrict n)
  __attribute__((nonnull));

#endif
