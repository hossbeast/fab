/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "moria/vertex.h"
#include "bam/ipc.h"
#include "bam/envs_tell.desc.h"
#include "narrator/growing.h"
#include "moria/foreach.h"
#include "bacon/value.h"
#include "bacon/builder.h"

#include "module_envs.h"
#include "graph.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "module.h"
#include "channel.h"
#include "edge.h"
#include "varset.h"
#include "exec.h"

#include "zcat.h"
#include "marshal.h"

static llist module_envs_list = LLIST_INITIALIZER(module_envs_list);          // active
static llist module_envs_freelist = LLIST_INITIALIZER(module_envs_freelist);  // free

module_envs *module_envs_create(struct module * restrict mod)
{
  module_envs *envs;

  if((envs = llist_shift(&module_envs_freelist, typeof(*envs), vertex.owner)) == 0) {
    xmalloc(&envs, sizeof(*envs));
  } else {
    memset(envs, 0, sizeof(*envs));
  }

  moria_vertex_init(&envs->vertex, &g_graph, VERTEX_MODULE_ENVS | VERTEX_OK);
  llist_prepend(&module_envs_list, &envs->vertex.owner);

  envs->mod = mod;

  vertex_protect_set(&envs->vertex, true);
  vertex_alloc_probe(&envs->vertex);

  return envs;
}

size_t module_envs_path_cat(void * restrict dst, size_t sz, const module_envs * restrict envs)
{
  return zcatf(dst, sz, "//module/%.*s/envs", envs->mod->id_len, envs->mod->id);
}

void module_envs_describe(const module_envs * restrict envs, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&envs->vertex, builder_writer);
    { SET
      vertex_mappings(&envs->vertex, builder_writer);
    }
  }
}

void module_envs_tell(const module_envs * restrict envs, struct channel * restrict channel)
{
  bamipc_message *msg;
  module *mod;
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

  mod = envs->mod;
  msg = channel_produce(channel);
  msg->type = BAMIPC_MSG_RESULT;
  msg->attrs = descriptor_bam_envs_tell_result.id;
  dst = msg->text;
  sz = sizeof(msg->text);
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_envs_tell_result.id);

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

    z += marshal_u32(dst + z, sz - z, descriptor_bam_envs_tell_item.id);

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

  /* finally, the effective envs text */
  if(mod->exec) {
    for(x = 0; x < mod->exec->envs_size; x++) {
      msg = channel_produce(channel);
      msg->type = BAMIPC_MSG_RESULT;
      msg->size = 0;
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, MMS(mod->exec->envs[x]));
      msg->size += zcatc(msg->text + msg->size, sizeof(msg->text) - msg->size, '\n');
      channel_post(channel, msg);
    }
  }
}
