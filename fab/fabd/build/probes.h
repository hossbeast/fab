#ifndef _BUILD_PROBES_H
#define _BUILD_PROBES_H

#include "types.h"

struct build_slot;

extern unsigned short build_stage_semaphore;
extern unsigned short build_fork_semaphore;
extern unsigned short build_wait_semaphore;

void build_stage_probe(uint16_t stage);

void build_fork_probe(uint16_t stage, uint16_t slot_index, const struct build_slot * restrict bs)
  __attribute__((nonnull));

void build_wait_probe(uint16_t stage, uint16_t slot_index, const struct build_slot * restrict bs)
  __attribute__((nonnull));

#endif
