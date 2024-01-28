/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/inotify.h>

#include "types.h"

#include "probes.h"
#include "sweeper_thread.h"
#include "moria.h"
#include "common/attrs.h"
#include "graph.h"
#include "vertex.h"

#include "usdt-sem.h"
#include "zcat.h"

PROBE_SEM(vertex, alloc) = 0;
PROBE_SEM(vertex, state) = 0;
PROBE_SEM(vertex, unlinked) = 0;
PROBE_SEM(vertex, deleted) = 0;

void vertex_alloc_probe(const moria_vertex * restrict v)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!vertex_alloc_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%-13s ", "alloc");
  z += vertex_identity_cat(s + z, sz - z, v);
  z += zcats(s + z, sz - z, " ");
  s[z] = 0;

printf("%.*s\n", (int)z, buf);
  PROBE2(vertex, alloc, buf, v);
}

void vertex_state_probe(const struct moria_vertex * restrict v, uint32_t prev)
{
  char buf[256];
  char *s;
  size_t sz;
  size_t z;

  if(!vertex_state_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%-13s ", "state");
  z += vertex_identity_cat(s + z, sz - z, v);
  z += zcats(s + z, sz - z, " was ");
  z += attrs32_zcat(s + z, sz - z, graph_vertex_attrs, prev);
  s[z] = 0;

printf("%.*s\n", (int)z, buf);
  PROBE3(vertex, state, s, v, s);
}

void vertex_unlinked_probe(const moria_vertex * restrict v)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!vertex_unlinked_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%-13s ", "unlink");
  z += vertex_identity_cat(s + z, sz - z, v);
  s[z] = 0;

printf("%.*s\n", (int)z, buf);
  PROBE2(vertex, unlinked, buf, v);
}

void vertex_deleted_probe(const moria_vertex * restrict v)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!vertex_deleted_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%-13s ", "free");
  z += vertex_identity_cat(s + z, sz - z, v);
  s[z] = 0;

printf("%.*s\n", (int)z, buf);
  PROBE2(vertex, deleted, buf, v);
}
