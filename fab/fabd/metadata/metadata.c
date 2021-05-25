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

#include <time.h>

#include "args.h"
#include "fab/metadata.h"
#include "goals.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "params.h"

#include "metadata.h"
#include "reconcile_thread.h"
#include "times.h"

#include "marshal.h"
#include "zbuffer.h"

xapi metadata_collate(void *dst, size_t sz, size_t * restrict zp)
{
  enter;

  size_t z;
  size_t off;
  struct timespec now;
  narrator *N;
  narrator_fixed fixed;

  clock_gettime(CLOCK_MONOTONIC, &now);

  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_fab_metadata.id);

  z += marshal_i64(dst + z, sz - z, g_params.pid);
  z += marshal_u64(dst + z, sz - z, timespec_delta(&g_params.starttime, &now));

  z += marshal_u16(dst + z, sz - z, strlen(g_params.ipcdir));
  z += znloads(dst + z, sz - z, g_params.ipcdir);

  z += marshal_u16(dst + z, sz - z, strlen(g_params.homedir));
  z += znloads(dst + z, sz - z, g_params.homedir);

  z += marshal_u16(dst + z, sz - z, strlen(g_params.proj_dir));
  z += znloads(dst + z, sz - z, g_params.proj_dir);

  z += marshal_u16(dst + z, sz - z, strlen(g_args.hash_str));
  z += znloads(dst + z, sz - z, g_args.hash_str);

  z += marshal_int(dst + z, sz - z, &global_system_state, sizeof(global_system_state));

  off = z;
  z += marshal_u16(dst + z, sz - z, 0);

  N = narrator_fixed_init(&fixed, dst + z, sz - z);
  fatal(goals_say, N);
  z += fixed.l;

  marshal_u16(dst + off, sz - off, fixed.l);

  z += marshal_u8(dst + z, sz - z, goals_autorun);

  *zp = z;

  finally : coda;
}
