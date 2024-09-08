/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "moria/vertex.h"
#include "bam/ipc.h"
#include "bam/vars_tell.desc.h"
#include "narrator/growing.h"
#include "moria/foreach.h"
#include "bacon/value.h"
#include "bacon/builder.h"

#include "module_vars.h"
#include "graph.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "module.h"
#include "channel.h"
#include "edge.h"
#include "varset.h"

#include "zcat.h"
#include "marshal.h"

static llist module_vars_list = LLIST_INITIALIZER(module_vars_list);          // active
static llist module_vars_freelist = LLIST_INITIALIZER(module_vars_freelist);  // free

module_vars *module_vars_create(struct module * restrict mod)
{
  module_vars *vars;

  if((vars = llist_shift(&module_vars_freelist, typeof(*vars), vertex.owner)) == 0) {
    xmalloc(&vars, sizeof(*vars));
  } else {
    memset(vars, 0, sizeof(*vars));
  }

  moria_vertex_init(&vars->vertex, &g_graph, VERTEX_MODULE_VARS | VERTEX_OK);
  llist_prepend(&module_vars_list, &vars->vertex.owner);

  vars->mod = mod;

  vertex_protect_set(&vars->vertex, true);
  vertex_alloc_probe(&vars->vertex);

  return vars;
}

size_t module_vars_path_cat(void * restrict dst, size_t sz, const module_vars * restrict vars)
{
  return zcatf(dst, sz, "//module/%.*s/vars", vars->mod->id_len, vars->mod->id);
}

void module_vars_describe(const module_vars * restrict vars, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&vars->vertex, builder_writer);
    { SET
      vertex_mappings(&vars->vertex, builder_writer);
    }
  }
}

void module_vars_tell(const module_vars * restrict vars, struct channel * restrict channel)
{
  bamipc_message *msg;
  module *mod;
  narrator_growing growing;
  narrator * N;
  int x;
  struct varset *var;
  moria_foreach_state st;
  moria_edge *edge;
  size_t z;
  size_t list_size;
  size_t list_size_off;
  size_t list_len_off;
  char *dst;
  size_t sz;

  mod = vars->mod;
  msg = channel_produce(channel);
  msg->type = BAMIPC_MSG_RESULT;
  msg->attrs = descriptor_bam_vars_tell_result.id;
  dst = msg->text;
  sz = sizeof(msg->text);
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_vars_tell_result.id);

  /* module id */
  z += marshal_u16(dst + z, sz - z, mod->id_len);
  z += zcatw(dst + z, sz - z, mod->id, mod->id_len);

  /* module abspath */
  z += marshal_u16(dst + z, sz - z, mod->dir_node_abspath_len);
  z += zcatw(dst + z, sz - z, mod->dir_node_abspath, mod->dir_node_abspath_len);

  /* list each contributing var.bam file */
  z += align(z, 4);
  list_size_off = z;
  z += marshal_u32(dst + z, sz - z, 0);            /* size of the list blob */
  z += align(z, 2);
  list_len_off = z;
  z += marshal_u16(dst + z, sz - z, 0);            /* items in the list blob */

  x = 0;
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    if(edge_type_get(edge) != EDGE_VARSET_MOD) {
      continue;
    }

    var = containerof(edge->B, typeof(*var), vertex);
    z += marshal_u32(dst + z, sz - z, descriptor_bam_vars_tell_item.id);

    /* file id */
    z += marshal_u16(dst + z, sz - z, var->id_len);
    z += zcatw(dst + z, sz - z, var->id, var->id_len);

    /* file path */
    z += marshal_u16(dst + z, sz - z, var->self_node_abspath_len);
    z += zcatw(dst + z, sz - z, var->self_node_abspath, var->self_node_abspath_len);
    x++;
  }
  list_size = z - list_size_off;
  marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
  marshal_u16(dst + list_len_off, sz - list_len_off, x);

  msg->size = z;
  channel_post(channel, msg);

  /* finally, the effective vars text */
  N = narrator_growing_init(&growing);
  bacon_value_say(mod->vars, N);

  x = 0;
  while(x < growing.l)
  {
    msg = channel_produce(channel);
    msg->type = BAMIPC_MSG_RESULT;
    msg->attrs = 0;
    msg->size = MIN(growing.l - x, sizeof(msg->text));
    memcpy(msg->text, growing.s + x, msg->size);
    channel_post(channel, msg);
    x += msg->size;
  }

  narrator_growing_destroy(&growing);
}
