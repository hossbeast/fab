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
#include "inotify_mask.h"

#include "usdt-sem.h"
#include "zcat.h"

#if 1
PROBE_SEM(notify_thread, event) = 0;
#else
PROBE_SEM(notify_thread, event) = 1;
#endif

void notify_thread_probe(const struct inotify_event * restrict ev, const fsent * restrict fse)
{
  char buf[512];
  char *s;
  size_t sz;
  size_t z;
  uint16_t len;

  if(!notify_thread_event_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "wd : %-8d ", ev->wd);
  z += zcats(s + z, sz - z, " event : ");
  z += inotify_mask_cat(s + z, sz - z, ev->mask);
  z += zcats(s + z, sz - z, " path : ");
  z += fsent_absolute_path_cat(s + z, sz - z, fse);
  if((len = strlen(ev->name))) {
    z += zcats(s + z, sz - z, "/");
    z += zcatw(s + z, sz - z, ev->name, len);
  }

  PROBE1(notify_thread, event, buf);
printf("%.*s\n", (int)z, buf);
}
