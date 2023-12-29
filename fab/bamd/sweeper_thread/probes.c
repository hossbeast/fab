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
#include "zcat.h"

#if 1
PROBE_SEM(sweeper_thread, event) = 0;
#else
PROBE_SEM(sweeper_thread, event) = 1;
#endif

void sweeper_thread_probe(const sweeper_event * restrict ev)
{
  char buf[512];
  char *s;
  size_t sz;
  size_t z;
  const char *event;
  fsent *n = 0;
  sweeper_child_event *cev = 0;

  if(!sweeper_thread_event_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  if(ev->kind == SWEEPER_EVENT_SELF)
  {
    n = containerof(ev, fsent, pending);
    cev = 0;
  }
  else if(ev->kind == SWEEPER_EVENT_CHILD)
  {
    cev = containerof(ev, typeof(*cev), sweep_event);
    n = cev->parent;
  }

  if(ev->mask & IN_DELETE)
  {
    /* entity was deleted */
    event = "DELETE";
  }
  else if(ev->kind == SWEEPER_EVENT_CHILD)
  {
    event = "CREATE";
  }
  else
  {
    event = "MODIFY";
  }

  z += zcatf(s + z, sz - z, " era %5"PRIu32" %9s ", ev->era, event);
  z += fsent_cwd_relative_path_cat(s + z, sz - z, n);
  if(cev)
  {
    z += zcats(s + z, sz - z, "//");
    z += zcatw(s + z, sz - z, cev->name, cev->name_len);
  }

  PROBE2(sweeper_thread, event, buf, ev);
printf("%.*s\n", (int)z, buf);
}
