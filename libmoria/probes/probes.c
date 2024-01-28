/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <string.h>

#include "types.h"

#include "probes.h"
#include "moria.h"

#include "usdt-sem.h"
#include "macros.h"
#include "zcat.h"

PROBE_SEM(moria, explore_edge) = 0;
PROBE_SEM(moria, explore_vertex) = 0;

void explore_edge_probe(moria_edge * restrict e, int distance, bool travel, bool visit)
{
  char buf[256];
  size_t z;
  size_t sz;
  char *s;
  int x;

  if(!moria_explore_edge_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  // in explore_edge
  z += zcatf(s + z, sz - z, "[e] %p d %d t %d v %d 0x%08x %*s ", e, distance, travel, visit, e->attrs, distance, "");
  if(e->attrs & MORIA_EDGE_HYPER) {
    z += zcatf(s + z, sz - z, "{");
    for(x = 0; x < e->Blen; x++) {
      z += zcatf(s + z, sz - z, " %.*s", e->Blist[x].v->label_len, e->Blist[x].v->label);
    }
    z += zcatf(s + z, sz - z, "} -> {");
    for(x = 0; x < e->Alen; x++) {
      z += zcatf(s + z, sz - z, " %.*s", e->Alist[x].v->label_len, e->Alist[x].v->label);
    }
    z += zcatf(s + z, sz - z, "}");
  } else {
    z += zcatf(s + z, sz - z, "%p %.*s -> %p %.*s", e->B, e->B->label_len, e->B->label, e->A, e->A->label_len, e->A->label);
  }

printf("%.*s\n", (int)z, buf);
  PROBE1(moria, explore_edge, buf);
}

// in explore_vertex
void explore_vertex_probe(moria_vertex * restrict v, int distance, bool travel, bool visit)
{
  char buf[256];
  size_t z;
  size_t sz;
  char *s;

  if(!moria_explore_vertex_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "[v] %p d %d t %d v %d 0x%08x %*s %.*s", v, distance, travel, visit, v->attrs, distance, "", v->label_len, v->label);

printf("%.*s\n", (int)z, buf);
  PROBE1(moria, explore_vertex, buf);
}
