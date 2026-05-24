/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria.h"
#include "foreach.h"

void moria_foreach_edge_up_init(moria_foreach_state * restrict s, const moria_vertex * restrict v)
{
//  memset(s, 0, sizeof(*s));

  if((s->rbn = rbtree_first_post(&v->up)) == &rbleaf) {
    s->next = v->up_identity;
  } else if((s->next = s->rbn->ud.p)) {
  } else {
    s->next = containerof(s->rbn, typeof(*s->next), rbn_up);
  }
}

void moria_foreach_edge_up_next(moria_foreach_state * restrict s, const moria_vertex * restrict v)
{
  if(s->next == v->up_identity) {
    s->next = 0;
  } else if((s->rbn = rbtree_next_post(s->rbn)) == &rbleaf) {
    s->next = v->up_identity;
  } else if((s->next = s->rbn->ud.p)) {
  } else {
    s->next = containerof(s->rbn, typeof(*s->next), rbn_up);
  }
}

void moria_foreach_edge_down_init(moria_foreach_state * restrict s, const moria_vertex * restrict v)
{
//  memset(s, 0, sizeof(*s));

  if((s->rbn = rbtree_first_post(&v->down)) == &rbleaf) {
    s->next = 0;
  } else if((s->next = s->rbn->ud.p)) {
  } else {
    s->next = containerof(s->rbn, typeof(*s->next), rbn_down);
  }
}

void moria_foreach_edge_down_next(moria_foreach_state * restrict s, const moria_vertex * restrict v)
{
  if((s->rbn = rbtree_next_post(s->rbn)) == &rbleaf) {
    s->next = 0;
  } else if((s->next = s->rbn->ud.p)) {
  } else {
    s->next = containerof(s->rbn, typeof(*s->next), rbn_down);
  }
}

void moria_foreach_edge_init(moria_foreach_state * restrict s, const moria_vertex * restrict v)
{
  moria_foreach_edge_up_init(s, v);
  if(s->next == 0) {
    moria_foreach_edge_down_init(s, v);
    s->down = true;
  } else {
    s->down = false;
  }
}

void moria_foreach_edge_next(moria_foreach_state * restrict s, const moria_vertex * restrict v)
{
  if(!s->down) {
    moria_foreach_edge_up_next(s, v);
    if(s->next == 0) {
      moria_foreach_edge_down_init(s, v);
      s->down = true;
    }
  } else {
    moria_foreach_edge_down_next(s, v);
  }
}
