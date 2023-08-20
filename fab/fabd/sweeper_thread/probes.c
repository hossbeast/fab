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

PROBE_SEM(sweeper, event);

void fsevent_probe(const sweeper_event * restrict ev)
{
  char buf[512];
  char *s;
  size_t sz;
  size_t z;
  const char *event;
  fsent *n = 0;
  sweeper_child_event *cev = 0;

  if(!sweeper_event_semaphore) {
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

  z += znloadf(s + z, sz - z, " era %5"PRIu32" %9s ", ev->era, event);
  z += fsent_project_relative_path_znload(s + z, sz - z, n);
  if(cev)
  {
    z += znloads(s + z, sz - z, "//");
    z += znloadw(s + z, sz - z, cev->name, cev->name_len);
  }

  PROBE2(sweeper, event, buf, ev);
}
