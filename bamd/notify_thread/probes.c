/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>

#include "types.h"

#include "probes.h"
#include "fsent.h"
#include "inotify_mask.h"

#include "usdt-sem.h"
#include "zcat.h"

PROBE_SEM(notify_thread, event) = 0;

void notify_thread_probe(const struct inotify_event * restrict ev, const fsent * restrict fse)
{
  char buf[PATH_MAX];
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
  z += zcatf(s + z, sz - z, " fse %p", fse);

printf("%.*s\n", (int)z, buf);
  PROBE1(notify_thread, event, buf);
}
