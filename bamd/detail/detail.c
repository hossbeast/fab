/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "bam/ipc.h"
#include "moria/foreach.h"
#include "bacon/value.h"
#include "bacon/builder.h"

#include "detail.h"
#include "graph.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "plugin.h"
#include "edge.h"
#include "varset.h"
#include "exec.h"
#include "request_handler.h"
#include "adaptor.h"
#include "plugin_process.h"
#include "formula_record.h"
#include "identity.h"

#include "bam/envs_tell.desc.h"
#include "bam/vars_tell.desc.h"

#include "zcat.h"
#include "marshal.h"

llist detail_list = LLIST_INITIALIZER(detail_list);                 // live
static llist detail_freelist = LLIST_INITIALIZER(detail_freelist);  // free

static void plugin_detail_tell(const detail * restrict stream, struct request_handler * restrict handler)
{
  struct plugin *plugin;
  struct plugin_process *process;
  struct adaptor adaptor;
  void *buf;
  size_t len;

  bamipc_message *msg;

  plugin = containerof(stream->subject, typeof(*plugin), vertex);
  if((process = plugin->process) == 0) {
    return;
  }

  switch(vertex_type_get(&stream->vertex)) {
    case VERTEX_PLUGIN_STDIN:
      adaptor.buf = process->in;
      adaptor.sz = process->in_size;
      adaptor.tail = &process->in_tail;
      adaptor.head = &process->in_head;
      break;
    case VERTEX_PLUGIN_STDOUT:
      adaptor.buf = process->out;
      adaptor.sz = process->out_size;
      adaptor.tail = &process->out_tail;
      adaptor.head = &process->out_head;
      break;
    case VERTEX_PLUGIN_STDERR:
      adaptor.buf = process->err;
      adaptor.sz = process->err_size;
      adaptor.tail = &process->err_tail;
      adaptor.head = &process->err_head;
      break;
    case VERTEX_PLUGIN_AUXOUT:
      adaptor.buf = process->auxout;
      adaptor.sz = process->auxout_size;
      adaptor.tail = &process->auxout_tail;
      adaptor.head = &process->auxout_head;
      break;
    default:
      RTA;
  }

  if((len = adaptor_dump0(&adaptor, &buf))) {
    msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_TEXT, 0);
    msg->attrs = 0;
    memcpy(msg->text, buf, len);
    msg->len = len;
    request_handler_post(handler, msg);
  }
  if((len = adaptor_dump1(&adaptor, &buf))) {
    msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_TEXT, 0);
    msg->attrs = 0;
    memcpy(msg->text, buf, len);
    msg->len = len;
    request_handler_post(handler, msg);
  }
}

static void module_envs_tell(const detail * restrict envs, struct request_handler * restrict handler)
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
  exec_render_context *ebc, ebcl;
  exec_builder *eb, ebl;

  mod = containerof(envs->subject, typeof(*mod), vertex);
  msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, descriptor_bam_envs_tell_result.id, 0);
  dst = msg->text;
  sz = msg->size;
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_envs_tell_result.id);

  /* module id */
  z += marshal_u16(dst + z, sz - z, mod->id_len);
  z += zcatw(dst + z, sz - z, mod->id, mod->id_len);

  /* module abspath */
  z += marshal_u16(dst + z, sz - z, mod->build_dir_abspath_len);
  z += zcatw(dst + z, sz - z, mod->build_dir_abspath, mod->build_dir_abspath_len);

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

  msg->len = z;
  request_handler_post(handler, msg);

  /* finally, the effective envs text */
  eb = &ebl;
  ebc = &ebcl;

  exec_builder_init(eb);
  exec_render_context_init(ebc);
  exec_render_context_configure(ebc, eb, mod->build_dir, mod, mod->vars, 0, 0);
  exec_render_env_vars(ebc);
  exec_builder_build(eb, 0);

  for(x = 0; x < eb->exec->envs_len; x++) {
    msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_TEXT, 0);
    msg->len = 0;
    msg->len += zcatw(msg->text + msg->len, msg->size - msg->len, MMS(eb->exec->envs[x]));
    msg->len += zcatc(msg->text + msg->len, msg->size - msg->len, '\n');
    request_handler_post(handler, msg);
  }

  exec_builder_destroy(eb);
  exec_render_context_destroy(ebc);
}

static void module_vars_tell(const detail * restrict vars, struct request_handler * restrict handler)
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

  mod = containerof(vars->subject, typeof(*mod), vertex);
  msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, descriptor_bam_vars_tell_result.id, 0);
  dst = msg->text;
  sz = msg->size;
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_vars_tell_result.id);

  /* module id */
  z += marshal_u16(dst + z, sz - z, mod->id_len);
  z += zcatw(dst + z, sz - z, mod->id, mod->id_len);

  /* module abspath */
  z += marshal_u16(dst + z, sz - z, mod->build_dir_abspath_len);
  z += zcatw(dst + z, sz - z, mod->build_dir_abspath, mod->build_dir_abspath_len);

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

  msg->len = z;
  request_handler_post(handler, msg);

  /* finally, the effective vars text */
  N = narrator_growing_init(&growing);
  bacon_value_say(mod->vars, N);

  x = 0;
  while(x < growing.l)
  {
    msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_TEXT, 0);
    msg->len = MIN(growing.l - x, msg->size);
    memcpy(msg->text, growing.s + x, msg->len);
    request_handler_post(handler, msg);
    x += msg->len;
  }

  narrator_growing_destroy(&growing);
}

detail *detail_alloc(moria_vertex *v, vertex_type type)
{
  struct detail *detail;
  identity_ctx id;

  if((detail = llist_shift(&detail_freelist, typeof(*detail), vertex.owner))) {
    ASAN_CLEAN(detail);
    memset(detail, 0, sizeof(*detail));
  } else {
    xmalloc(&detail, sizeof(*detail));
  }

  moria_vertex_init(&detail->vertex, &g_graph, type | VERTEX_OK);
  detail->subject = v;

  identity_init(&id);
  detail_compute_id(detail, &id);
  detail->id_len = identity_mk(&id, detail->id, sizeof(detail->id));

  vertex_alloc_probe(&detail->vertex);
  llist_append(&detail_list, &detail->vertex.owner);

  return detail;
}

void detail_release(struct detail * restrict detail)
{
  moria_vertex __attribute__((unused)) *v;

  /* should be fully excised from the graph previously */
  v = &detail->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  llist_delete(&detail->vertex.owner);
  llist_append(&detail_freelist, &detail->vertex.owner);
  ASAN_TAINT(detail);
}

size_t detail_path_cat(void * restrict dst, size_t sz, const struct detail * restrict detail)
{
  const struct formula_record *formula_record;
  const struct plugin *plugin;
  const struct module *module;

  formula_record = containerof(detail->subject, typeof(*formula_record), vertex);
  plugin = containerof(detail->subject, typeof(*plugin), vertex);
  module = containerof(detail->subject, typeof(*module), vertex);

  switch(vertex_type_get(&detail->vertex)) {
    case VERTEX_FORMULA_STDIN:
      return zcatf(dst, sz, "//bm/formula-records/%.*s/stdin", formula_record->id_len, formula_record->id);
    case VERTEX_FORMULA_STDOUT:
      return zcatf(dst, sz, "//bm/formula-records/%.*s/stdout", formula_record->id_len, formula_record->id);
    case VERTEX_FORMULA_STDERR:
      return zcatf(dst, sz, "//bm/formula-records/%.*s/stderr", formula_record->id_len, formula_record->id);
    case VERTEX_FORMULA_AUXOUT:
      return zcatf(dst, sz, "//bm/formula-records/%.*s/auxout", formula_record->id_len, formula_record->id);
    case VERTEX_FORMULA_ARGS:
      return zcatf(dst, sz, "//bm/formula-records/%.*s/args", formula_record->id_len, formula_record->id);
    case VERTEX_FORMULA_ENVS:
      return zcatf(dst, sz, "//bm/formula-records/%.*s/envs", formula_record->id_len, formula_record->id);
    case VERTEX_FORMULA_FILE:
      return zcatf(dst, sz, "//bm/formula-records/%.*s/file", formula_record->id_len, formula_record->id);
    case VERTEX_FORMULA_CWD:
      return zcatf(dst, sz, "//bm/formula-records/%.*s/cwd", formula_record->id_len, formula_record->id);
    case VERTEX_PLUGIN_STDIN:
      return zcatf(dst, sz, "//bm/plugins/%.*s/stdin", plugin->id_len, plugin->id);
    case VERTEX_PLUGIN_STDOUT:
      return zcatf(dst, sz, "//bm/plugins/%.*s/stdout", plugin->id_len, plugin->id);
    case VERTEX_PLUGIN_STDERR:
      return zcatf(dst, sz, "//bm/plugins/%.*s/stderr", plugin->id_len, plugin->id);
    case VERTEX_PLUGIN_AUXOUT:
      return zcatf(dst, sz, "//bm/plugins/%.*s/auxout", plugin->id_len, plugin->id);
    case VERTEX_MODULE_ENVS:
      return zcatf(dst, sz, "//bm/modules/%.*s/envs", module->id_len, module->id);
    case VERTEX_MODULE_VARS:
      return zcatf(dst, sz, "//bm/modules/%.*s/vars", module->id_len, module->id);
    default:
      RTA;
  }
}

void detail_describe(const detail * restrict stream, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&stream->vertex, builder_writer);
    { SET
      vertex_mappings(&stream->vertex, builder_writer);
      vertex_relations_describe(&stream->vertex, builder_writer);
    }
  }
}

void detail_tell(const detail * restrict detail, struct request_handler * restrict handler)
{
  switch(vertex_type_get(&detail->vertex)) {
    case VERTEX_FORMULA_STDIN:
    case VERTEX_FORMULA_STDERR:
    case VERTEX_FORMULA_STDOUT:
    case VERTEX_FORMULA_AUXOUT:
    case VERTEX_FORMULA_ARGS:
    case VERTEX_FORMULA_ENVS:
    case VERTEX_FORMULA_FILE:
    case VERTEX_FORMULA_CWD:
      return formula_record_detail_tell(detail, handler);
    case VERTEX_PLUGIN_STDIN:
    case VERTEX_PLUGIN_STDERR:
    case VERTEX_PLUGIN_STDOUT:
    case VERTEX_PLUGIN_AUXOUT:
      return plugin_detail_tell(detail, handler);
    case VERTEX_MODULE_ENVS:
      return module_envs_tell(detail, handler);
    case VERTEX_MODULE_VARS:
      return module_vars_tell(detail, handler);
    default:
      RTA;
  }
}

void detail_cleanup()
{
  detail *v;

  while((v = llist_shift(&detail_freelist, typeof(*v), vertex.owner))) {
    ASAN_CLEAN(v);
    free(v);
  }
}

void detail_show(void)
{
  detail *fml;
  int x;

  printf("details, live : %zu free : %zu\n", llist_count(&detail_list), llist_count(&detail_freelist));
  x = 0;
  llist_foreach(&detail_list, fml, vertex.owner) {
    printf("%3u ", x++);
    vertex_identity_show(&fml->vertex);
    vertex_edges_show(&fml->vertex);
  }
}

bool detail_reap(struct detail * restrict fsn, struct graph_disintegrate_context * restrict ctx)
{
  return true;
}

void detail_hash(const struct detail * restrict detail, const void ** restrict h, uint16_t * restrict hl)
{
  *h = &detail->hash;
  *hl = sizeof(detail->hash);
}

void detail_id(const struct detail * restrict detail, const char ** restrict id, uint16_t * restrict idl)
{
  *id = detail->id;
  *idl = detail->id_len;
}

void detail_compute_id(const struct detail * restrict detail, struct identity_ctx * restrict id)
{
  /* subject */
  vertex_compute_id(detail->subject, id);
  identity_add(id, "\n", 1);

  /* type */
  identity_type(id, vertex_type_get(&detail->vertex));
}
