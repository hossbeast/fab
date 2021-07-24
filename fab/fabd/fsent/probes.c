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
