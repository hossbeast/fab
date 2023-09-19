/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "types.h"

#include "probes.h"
#include "build_slot.h"
#include "fsent.h"

#include "usdt-sem.h"
#include "zbuffer.h"

PROBE_SEM(build, stage);
PROBE_SEM(build, fork);
PROBE_SEM(build, wait);

void build_stage_probe(uint16_t stage)
{
  char buf[512];
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
    z += znloadf(s + z, sz - z, "stage begin %3d", stage);
  } else {
    z += znloadf(s + z, sz - z, "stage advance %3d -> %3d", stage - 1, stage);
  }

  PROBE1(build, stage, buf);
}

void build_fork_probe(uint16_t stage, uint16_t slot_index, const struct build_slot * restrict bs)
{
  char buf[512];
  char *s;
  size_t sz;
  size_t z;

  if(!build_fork_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += znloadf(s + z, sz - z, "%6s slot %3"PRIu32" stage %3d index %3d", "launch", (uint32_t)slot_index, stage, bs->stage_index);

  PROBE4(build, fork, buf, stage, slot_index, bs);
}

void build_wait_probe(uint16_t stage, uint16_t slot_index, const struct build_slot * restrict bs)
{
  char buf[512];
  char *s;
  size_t sz;
  size_t z;

  if(!build_wait_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += znloadf(s + z, sz - z, "%6s slot %3"PRIu32" stage %3d index %3d status %3d stderr-len %3d"
    , "reap"
    , (uint32_t)slot_index
    , stage
    , bs->stage_index
    , bs->status
    , bs->stderr_total
  );

  PROBE4(build, fork, buf, stage, slot_index, bs);
}
