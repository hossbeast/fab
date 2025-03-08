/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/llist.h"

#include "moria.h"

#include "linear_search.h"
#include "membership.h"

void moria_graph_linear_search(
    llist * restrict vertex_list
  , const char * restrict label
  , uint16_t label_len
  , const uint32_t *mask_list
  , const uint32_t *bits_list
  , uint16_t mask_list_len
  , moria_vertex ** restrict rv
)
{
  moria_vertex *v;

  *rv = 0;
  llist_foreach(vertex_list, v, owner) {
    if(!membership_vertex(v, mask_list, bits_list, mask_list_len)) {
      continue;
    }
    if(memncmp(v->label, v->label_len, label, label_len) == 0) {
      if(*rv) {
        RTA;
      }
      *rv = v;
    }
  }
}
