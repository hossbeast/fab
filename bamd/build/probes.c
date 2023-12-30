/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "types.h"

#include "probes.h"
#include "build_slot.h"
#include "fsent.h"

#include "usdt-sem.h"
#include "zcat.h"

#if 1
PROBE_SEM(build, stage) = 1;
PROBE_SEM(build, fork) = 1;
PROBE_SEM(build, wait) = 1;
#else
PROBE_SEM(build, stage) = 0;
PROBE_SEM(build, fork) = 0;
PROBE_SEM(build, wait) = 0;
#endif

void build_stage_probe(uint16_t stage)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!build_stage_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  if(stage == 0) {
    z += zcatf(s + z, sz - z, "stage begin %3d", stage);
  } else {
    z += zcatf(s + z, sz - z, "stage advance %3d -> %3d", stage - 1, stage);
  }

printf("%.*s\n", (int)z, buf);
  PROBE1(build, stage, buf);
}

void build_fork_probe(uint16_t stage, uint16_t slot_index, const struct build_slot * restrict bs)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!build_fork_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%6s slot %3"PRIu32" stage %3d index %3d pid %6u", "launch", (uint32_t)slot_index, stage, bs->stage_index, bs->pid);

printf("%.*s\n", (int)z, buf);
  PROBE4(build, fork, buf, stage, slot_index, bs);
}

void build_wait_probe(uint16_t stage, uint16_t slot_index, const struct build_slot * restrict bs)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!build_wait_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%6s slot %3"PRIu32" stage %3d index %3d pid %6u status %3d stderr-len %3d"
    , "reap"
    , (uint32_t)slot_index
    , stage
    , bs->stage_index
    , bs->pid
    , bs->status
    , bs->stderr_total
  );

printf("%.*s\n", (int)z, buf);
  PROBE4(build, fork, buf, stage, slot_index, bs);
}
