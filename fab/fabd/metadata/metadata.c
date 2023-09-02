/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <time.h>

#include "args.h"
#include "fab/metadata.h"
#include "goals.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "params.h"
#include "fsent.h"

#include "metadata.h"
#include "run_thread.h"
#include "times.h"
#include "system_state.h"

#include "marshal.h"
#include "zcat.h"

void metadata_collate(void *dst, size_t sz, size_t * restrict zp)
{
  size_t z;
  size_t off;
  struct timespec now;
  narrator *N;
  narrator_fixed fixed;

  clock_gettime(CLOCK_MONOTONIC, &now);

  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_fab_metadata.id);

  /* pid */
  z += marshal_i64(dst + z, sz - z, g_params.pid);
  /* uptime */
  z += marshal_u64(dst + z, sz - z, timespec_delta(&g_params.starttime, &now));
  /* ipcdir */
  z += marshal_u16(dst + z, sz - z, strlen(g_params.ipcdir));
  z += zcats(dst + z, sz - z, g_params.ipcdir);
  /* homedir */
  z += marshal_u16(dst + z, sz - z, strlen(g_params.homedir));
  z += zcats(dst + z, sz - z, g_params.homedir);
  /* cwd */
  z += marshal_u16(dst + z, sz - z, strlen(g_cwd_path));
  z += zcatw(dst + z, sz - z, g_cwd_path, g_cwd_path_len);
  /* project hash */
  z += marshal_u16(dst + z, sz - z, strlen(g_args.hash_str));
  z += zcats(dst + z, sz - z, g_args.hash_str);
  /* system state */
  z += marshal_int(dst + z, sz - z, &system_state, sizeof(system_state));
  /* system error */
  z += marshal_int(dst + z, sz - z, &system_error, sizeof(system_error));

  /* goals */
  off = z;
  z += marshal_u16(dst + z, sz - z, 0);

  N = narrator_fixed_init(&fixed, dst + z, sz - z);
  goals_say(N);
  z += fixed.l;

  marshal_u16(dst + off, sz - off, fixed.l);

  /* autorun */
  z += marshal_u8(dst + z, sz - z, run_thread_autorun);

  *zp = z;
}
