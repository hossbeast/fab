#ifndef _PROBES_H
#define _PROBES_H

#include "types.h"

struct sweeper_event;

extern unsigned short fsevent_semaphore;

void fsevent_probe(const struct sweeper_event * restrict ev)
  __attribute__((nonnull));

#endif
