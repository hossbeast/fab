/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria.h"
#include "bam/ipc.h"
#include "bam/list.desc.h"
#include "common/attrs.h"

#include "tree_list.h"
#include "graph.h"
#include "vertex.h"
#include "handler.h"
#include "request.h"
#include "fsent.h"
#include "channel.h"

#include "marshal.h"
#include "zcat.h"

static enum vertex_state tree_vertex_state_get(const moria_vertex * restrict v)
{
  switch(vertex_state_get(v)) {
    case VERTEX_UNCREATED:
      return VERTEX_UNCREATED;
    case VERTEX_UNLINKED:
      return VERTEX_UNLINKED;
    default:
      break;
  }

  if(vertex_invalid_get(v)) {
    return VERTEX_INVALID;
  }

  return vertex_state_get(v);
}

void tree_list_handler(handler_context * restrict ctx, command * restrict cmd)
{
  selected *sn;
  moria_vertex *v;
  fsent *n;
  size_t z;
  bamipc_message *msg;
  vertex_state state;
  char *dst;
  size_t sz;
  uint16_t len;
  uint16_t off;
  const char *s;

  llist_foreach(&ctx->selection->list, sn, lln) {
    v = sn->v;
    msg = channel_produce(ctx->chan);
    msg->type = BAMIPC_MSG_RESULT;

    dst = msg->text;
    sz = sizeof(msg->text);

    z = 0;
    z += marshal_u32(dst + z, sz - z, descriptor_bam_list_item.id);

    /* type */
    s = attrs32_name_byvalue(graph_vertex_type_attrs, vertex_type_get(v));
    z += marshal_u16(dst + z, sz - z, strlen(s));
    z += zcats(dst + z, sz - z, s);

    /* state */
    state = tree_vertex_state_get(v);
    s = attrs32_name_byvalue(graph_vertex_state_attrs, state);
    z += marshal_u16(dst + z, sz - z, strlen(s));
    z += zcats(dst + z, sz - z, s);

    /* label */
    z += marshal_u16(dst + z, sz - z, v->label_len);
    z += zcatw(dst + z, sz - z, v->label, v->label_len);

    /* abspath */
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    if(vertex_kind_get(v) == VERTEX_KIND_FSENT)
    {
      n = containerof(v, typeof(*n), vertex);
      len = fsent_absolute_path_cat(dst + z, sz - z, n);
      z += len;
      marshal_u16(dst + off, sz - off, len);
    }

    /* ref */
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    switch(vertex_type_get(v)) {
      case VERTEX_SYMLINK:
      case VERTEX_SHADOW_LINK:
        if(v->ref) {
          len = vertex_path_cat(dst + z, sz - z, v->ref);
          z += len;
          marshal_u16(dst + off, sz - off, len);
        }
        break;
      default:
        break;
    }

    /* distance */
    z += marshal_u16(dst + z, sz - z, sn->rank);

    msg->size = z;
    channel_post(ctx->chan, msg);
  }
}
