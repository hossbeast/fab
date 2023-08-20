/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/inotify.h>

#include "types.h"

#include "probes.h"
#include "sweeper_thread.h"
#include "fsent.h"

#include "usdt-sem.h"
#include "zbuffer.h"

PROBE_SEM(fsent, deleted);

void fsent_deleted_probe(const fsent * restrict n)
{
  char buf[512];
  char *s;
  size_t sz;
  size_t z;

  if(!fsent_deleted_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += znloadf(s + z, sz - z, "delete %p %s", n, n->vertex.label);

  PROBE2(fsent, deleted, buf, n);
}
