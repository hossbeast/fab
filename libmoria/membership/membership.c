/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria.h"
#include "membership.h"

bool membership_vertex(
    const moria_vertex * restrict v
  , const uint32_t * restrict mask_list
  , const uint32_t * restrict bits_list
  , uint16_t mask_list_len
)
{
  int x;

  for(x = 0; x < mask_list_len; x++) {
    if((v->attrs & mask_list[x]) != bits_list[x]) {
      return false;
    }
  }

  return true;
}

bool membership_edge(
    const moria_edge * restrict e
  , const uint32_t * restrict mask_list
  , const uint32_t * restrict bits_list
  , uint16_t mask_list_len
)
{
  int x;

  if(e->attrs & MORIA_EDGE_HYPER)
  {
    for(x = 0; x < e->Alen; x++) {
      if(!membership_vertex(e->Alist[x].v, mask_list, bits_list, mask_list_len)) {
        return false;
      }
    }
    for(x = 0; x < e->Blen; x++) {
      if(!membership_vertex(e->Blist[x].v, mask_list, bits_list, mask_list_len)) {
        return false;
      }
    }
  }
  else
  {
    if(!membership_vertex(e->A, mask_list, bits_list, mask_list_len) || !membership_vertex(e->B, mask_list, bits_list, mask_list_len)) {
      return false;
    }
  }

  return true;
}
