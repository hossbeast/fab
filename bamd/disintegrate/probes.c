/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/llist.h"
#include "moria.h"

#include "disintegrate.h"
#include "disintegrate/probes.h"
#include "vertices.h"
#include "edges.h"
#include "vertex.h"
#include "edge.h"

static bool disintegrate;
static bool disintegrate_round;
static bool disintegrate_reap;
static bool disintegrate_keep_unlinked;
static bool disintegrate_keep_exists;
static bool disintegrate_keep_has_down;
static bool disintegrate_keep_has_up;

static int counter;
static int activation;
static int round;

void disintegrate_probe(graph_disintegrate_context * restrict ctx)
{
  if(!disintegrate) {

  }

  activation++;
  round = 0;
}

void disintegrate_round_probe(graph_disintegrate_context * restrict ctx)
{
  struct vertices *vi;
  struct edges *ei;
  moria_vertex *v;
  moria_edge *e;

  if(!disintegrate_round) {
    return;
  }

  counter++;
  round++;
  printf("%3d activation %d round %3d unlinking (%zu) [\n", counter, activation, round, llist_count(&ctx->unlinking->lln));
  vertices_foreach(ctx->unlinking, v, vi) {
    printf(" ");
    vertex_identity_show(v);
  }
  printf("]\n");
  printf("%3d activation %d round %3d releasing (%zu) [\n", counter, activation, round, llist_count(&ctx->releasing->lln));
  vertices_foreach(ctx->releasing, v, vi) {
    printf(" ");
    vertex_identity_show(v);
  }
  printf("]\n");
  printf("%3d activation %d round %5d edges (%zu) [\n", counter, activation, round, llist_count(&ctx->edges->lln));
  edges_foreach(ctx->edges, e, ei) {
    printf(" ");
    edge_identity_show(e);
  }
  printf("]\n");
}

void disintegrate_reap_probe(const moria_vertex * restrict v)
{
  if(!disintegrate_reap) {
    return;
  }

  printf("%3d REAP %p\n", counter, v);
  vertex_identity_show(v);
}

void disintegrate_keep_unlinked_probe(const moria_vertex * restrict v)
{
  if(!disintegrate_keep_unlinked) {
    return;
  }

  printf("%3d keep %p already UNLINKED\n", counter, v);
  vertex_identity_show(v);
}

void disintegrate_keep_exists_probe(const moria_vertex * restrict v)
{
  if(!disintegrate_keep_exists) {
    return;
  }

  printf("%3d keep %p still EXISTS\n", counter, v);
  vertex_identity_show(v);
}

void disintegrate_keep_has_up_probe(const moria_vertex * restrict v)
{
  if(!disintegrate_keep_has_up) {
    return;
  }

  printf("%3d keep %p has UP\n", counter, v);
  vertex_identity_show(v);
}

void disintegrate_keep_has_down_probe(const moria_vertex * restrict v)
{
  if(!disintegrate_keep_has_down) {
    return;
  }

  printf("%3d keep %p has DOWN\n", counter, v);
  vertex_identity_show(v);
}
