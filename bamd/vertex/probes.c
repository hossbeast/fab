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
PROBE_SEM(vertex, unlink) = 0;
PROBE_SEM(vertex, free) = 0;

static uint32_t alloc_counter;
static uint32_t state_counter;
static uint32_t unlink_counter;
static uint32_t free_counter;

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

  z += zcatf(s + z, sz - z, "%8u ", ++alloc_counter);
  z += zcatf(s + z, sz - z, "%-13s ", "alloc");
  z += vertex_identity_cat(s + z, sz - z, v);
  s[z] = 0;

printf("%.*s\n", (int)z, buf);
  PROBE2(vertex, alloc, buf, v);
}

void vertex_state_probe(const struct moria_vertex * restrict v, uint32_t prev)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!vertex_state_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%8u ", ++state_counter);
  z += zcatf(s + z, sz - z, "%-13s ", "state");
  z += vertex_identity_cat(s + z, sz - z, v);
  z += zcats(s + z, sz - z, " was ");
  z += attrs32_zcat(s + z, sz - z, graph_vertex_attrs, prev);
  s[z] = 0;

printf("%.*s\n", (int)z, buf);
  PROBE3(vertex, state, s, v, s);
}

void vertex_unlink_probe(const moria_vertex * restrict v)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!vertex_unlink_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%8u ", ++unlink_counter);
  z += zcatf(s + z, sz - z, "%-13s ", "unlink");
  z += vertex_identity_cat(s + z, sz - z, v);
  s[z] = 0;

printf("%.*s\n", (int)z, buf);
  PROBE2(vertex, unlink, buf, v);
}

void vertex_free_probe(const moria_vertex * restrict v)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!vertex_free_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%8u ", ++free_counter);
  z += zcatf(s + z, sz - z, "%-13s ", "free");
  z += vertex_identity_cat(s + z, sz - z, v);
  s[z] = 0;

printf("%.*s\n", (int)z, buf);
  PROBE2(vertex, delete, buf, v);
}
