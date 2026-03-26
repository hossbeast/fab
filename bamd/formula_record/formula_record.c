/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/llist.h"
#include "xlinux/xstdlib.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "moria/vertex.h"
#include "moria/foreach.h"
#include "moria/edge.h"
#include "moria/connect.h"
#include "bam/ipc.h"

#include "formula_record.h"
#include "vertex.h"
#include "exec.h"
#include "times.h"
#include "graph.h"
#include "shadow.h"
#include "edges.h"
#include "vertices.h"
#include "edge.h"
#include "fsedge.h"
#include "disintegrate.h"
#include "build.h"
#include "vertex/probes.h"
#include "special_edge.h"
#include "fsent.h"
#include "detail.h"
#include "adaptor.h"
#include "request_handler.h"
#include "identity.h"
#include "build_record.h"
#include "bpe.h"

llist formula_record_list = LLIST_INITIALIZER(formula_record_list);
static llist freelist = LLIST_INITIALIZER(freelist);

static void formula_record_init(formula_record *rec)
{
  memset(rec, 0, offsetof(typeof(*rec), _retain));

  xrealloc(&rec->stdout_buf, 1, build_system_stdout_size, rec->stdout_size);
  rec->stdout_size = build_system_stdout_size;

  xrealloc(&rec->stderr_buf, 1, build_system_stderr_size, rec->stderr_size);
  rec->stderr_size = build_system_stderr_size;

  xrealloc(&rec->auxout_buf, 1, build_system_auxout_size, rec->auxout_size);
  rec->auxout_size = build_system_auxout_size;
}

static void formula_record_destroy(formula_record *rec)
{
  free(rec->stdout_buf);
  free(rec->stderr_buf);
  free(rec->auxout_buf);
}

formula_record *formula_record_alloc(build_record * restrict build_rec, buildplan_entity * restrict bpe)
{
  formula_record *rec;
  graph_invalidation_context invalidation = { };
  struct detail *detail;
  identity_ctx id;

  if((rec = llist_shift(&freelist, typeof(*rec), vertex.owner))) {
    ASAN_CLEAN(rec);
  } else {
    xmalloc(&rec, sizeof(*rec));
  }
  formula_record_init(rec);

  rec->build_rec = build_rec;
  rec->bpe = bpe;

  identity_init(&id);
  formula_record_compute_id(rec, &id);
  rec->id_len = identity_mk(&id, rec->id, sizeof(rec->id));

  moria_vertex_initw(&rec->vertex, &g_graph, VERTEX_FORMULA_RECORD | VERTEX_OK, rec->id, rec->id_len);

  llist_append(&formula_record_list, &rec->vertex.owner);

  vertex_alloc_probe(&rec->vertex);

  /* build out the shadow dir */
  graph_invalidation_begin(&invalidation);
  shadow_create(g_shadow_formula_records, &rec->shadow, rec->id, rec->id_len, VERTEX_SHADOW_DIR, &invalidation);

  /* enable traversal via specials graph */
  special_connect(&rec->shadow->vertex, &rec->vertex, EDGE_SPECIAL, &invalidation);
  graph_invalidation_end(&invalidation);

  /* other nodes */
  detail = detail_alloc(&rec->vertex, VERTEX_FORMULA_STDIN);
  shadow_mkref(rec->shadow, MMS("stdin"), &detail->vertex);
  detail = detail_alloc(&rec->vertex, VERTEX_FORMULA_STDOUT);
  shadow_mkref(rec->shadow, MMS("stdout"), &detail->vertex);
  detail = detail_alloc(&rec->vertex, VERTEX_FORMULA_STDERR);
  shadow_mkref(rec->shadow, MMS("stderr"), &detail->vertex);
  detail = detail_alloc(&rec->vertex, VERTEX_FORMULA_AUXOUT);
  shadow_mkref(rec->shadow, MMS("auxout"), &detail->vertex);
  detail = detail_alloc(&rec->vertex, VERTEX_FORMULA_ARGS);
  shadow_mkref(rec->shadow, MMS("args"), &detail->vertex);
  detail = detail_alloc(&rec->vertex, VERTEX_FORMULA_ENVS);
  shadow_mkref(rec->shadow, MMS("envs"), &detail->vertex);
  detail = detail_alloc(&rec->vertex, VERTEX_FORMULA_FILE);
  shadow_mkref(rec->shadow, MMS("file"), &detail->vertex);
  detail = detail_alloc(&rec->vertex, VERTEX_FORMULA_CWD);
  shadow_mkref(rec->shadow, MMS("cwd"), &detail->vertex);

  return rec;
}

void formula_record_setup()
{
}

void formula_record_cleanup()
{
  formula_record *rec;

  while((rec = llist_shift(&formula_record_list, typeof(*rec), vertex.owner))) {
    graph_demolish(&rec->vertex);
  }

  while((rec = llist_shift(&freelist, typeof(*rec), vertex.owner))) {
    ASAN_CLEAN(rec);
    formula_record_destroy(rec);
    free(rec);
  }
}

void formula_record_describe(const formula_record * restrict rec, bacon_writer * restrict builder_writer)
{
  char buf[128];

  { MAPPING
    vertex_path_describe(&rec->vertex, builder_writer);
    { SET
      vertex_mappings(&rec->vertex, builder_writer);
      { MAPPING STRING("pid") UINT(rec->pid) }
      { MAPPING STRING("status") UINT(rec->status) }
      { MAPPING STRING("stage") UINT(rec->stage) }
      { MAPPING STRING("stage-index") UINT(rec->stage_index) }
      { MAPPING STRING("stage-count") UINT(rec->stage_count) }
      { MAPPING STRING("runtime")
        timespec_ns_cat(buf, sizeof(buf), timespec_delta(&rec->forked_time, &rec->waited_time));
        STRING(buf)
      }
      { MAPPING STRING("stdout")
        { SET
          { MAPPING STRING("size") UINT(rec->stdout_size) }
          { MAPPING STRING("avail") UINT(rec->stdout_tail - rec->stdout_head) }
          { MAPPING STRING("total") UINT(rec->stdout_total) }
#if CONFIG_DEVEL
          { MAPPING STRING("head") UINT(rec->stdout_head) }
          { MAPPING STRING("tail") UINT(rec->stdout_tail) }
#endif
        }
      }
      { MAPPING STRING("stderr")
        { SET
          { MAPPING STRING("size") UINT(rec->stderr_size) }
          { MAPPING STRING("avail") UINT(rec->stderr_tail - rec->stderr_head) }
          { MAPPING STRING("total") UINT(rec->stderr_total) }
#if CONFIG_DEVEL
          { MAPPING STRING("head") UINT(rec->stderr_head) }
          { MAPPING STRING("tail") UINT(rec->stderr_tail) }
#endif
        }
      }
      { MAPPING STRING("auxout")
        { SET
          { MAPPING STRING("size") UINT(rec->auxout_size) }
          { MAPPING STRING("avail") UINT(rec->auxout_tail - rec->auxout_head) }
          { MAPPING STRING("total") UINT(rec->auxout_total) }
#if CONFIG_DEVEL
          { MAPPING STRING("head") UINT(rec->auxout_head) }
          { MAPPING STRING("tail") UINT(rec->auxout_tail) }
#endif
        }
      }
      { MAPPING STRING("exec")
        exec_describe(rec->exec, builder_writer);
      }
      vertex_relations_describe(&rec->vertex, builder_writer);
    }
  }
}

void formula_record_identity_describe(const formula_record * restrict rec, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(rec->vertex.attrs, graph_vertex_attrs, builder_writer);
  { MAPPING
    STRING("id");
    BYTES(rec->id, rec->id_len);
  }
}

size_t formula_record_path_cat(void * restrict dst, size_t sz, const formula_record * restrict rec)
{
  return zcatf(dst, sz, "//bm/formula-records/%.*s", rec->id_len, rec->id);
}

void formula_record_stats_build(formula_record * restrict rec, bool reset, struct bacon_writer * restrict writer)
{

}

void formula_record_release(formula_record * restrict rec)
{
  moria_vertex __attribute__((unused)) *v;

  exec_free(rec->exec);
  rec->exec = 0;

  /* must have been fully excised from the graph previously */
  v = &rec->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  llist_delete(&rec->vertex.owner);
  llist_append(&freelist, &rec->vertex.owner);
  ASAN_TAINT(rec);
}

void formula_record_unlink(formula_record * restrict rec, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  moria_foreach_edge(&st, &rec->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_FML_REC_BPE:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

static size_t args_tell(const formula_record * restrict rec, bamipc_message * restrict msg)
{
  char * const *s;
  char *dst;
  size_t sz;
  size_t z;

  dst = msg->text;
  sz = msg->size;
  z = 0;

  for(s = rec->exec->args; *s; s++) {
    z += zcats(dst + z, sz - z, *s);
  }

  return z;
}

static size_t envs_tell(const formula_record * restrict rec, bamipc_message * restrict msg)
{
  char * const *s;
  char *dst;
  size_t sz;
  size_t z;

  dst = msg->text;
  sz = msg->size;
  z = 0;

  for(s = rec->exec->envs; *s; s++) {
    if(s != rec->exec->envs) {
      z += zcatc(dst + z, sz - z, '\n');
    }
    z += zcats(dst + z, sz - z, *s);
  }

  return z;
}

static size_t file_tell(const formula_record * restrict rec, bamipc_message * restrict msg)
{
  char *dst;
  size_t sz;
  size_t z;

  dst = msg->text;
  sz = msg->size;
  z = 0;

  z += zcatw(dst + z, sz - z, rec->exec->file_path, rec->exec->file_path_len);

  return z;
}

static size_t cwd_tell(const formula_record * restrict rec, bamipc_message * restrict msg)
{
  char *dst;
  size_t sz;
  size_t z;

  dst = msg->text;
  sz = msg->size;
  z = 0;

  z += zcatw(dst + z, sz - z, rec->exec->cwd, rec->exec->cwd_len);

  return z;
}

static void formula_tell(const detail * restrict detail, struct request_handler * restrict handler)
{
  formula_record *rec;
  bamipc_message *msg;
  char *dst;
  size_t sz;
  size_t z;

  msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_TEXT, 0);
  msg->attrs = 0;

  dst = msg->text;
  sz = msg->size;
  z = 0;

  rec = containerof(detail->subject, typeof(*rec), vertex);
  z += zcatw(dst + z, sz - z, rec->exec->cwd, rec->exec->cwd_len);

  switch(vertex_type_get(&detail->vertex)) {
    case VERTEX_FORMULA_ARGS:
      z = args_tell(rec, msg);
      break;
    case VERTEX_FORMULA_ENVS:
      z = envs_tell(rec, msg);
      break;
    case VERTEX_FORMULA_FILE:
      z = file_tell(rec, msg);
      break;
    case VERTEX_FORMULA_CWD:
      z = cwd_tell(rec, msg);
      break;
    default:
      RTA;
  }

  msg->len = z;
  request_handler_post(handler, msg);
}

void formula_record_detail_tell(const detail * restrict detail, struct request_handler * restrict handler)
{
  adaptor adaptor;
  formula_record *rec;
  void *buf;
  size_t len;
  bamipc_message *msg;

  rec = containerof(detail->subject, typeof(*rec), vertex);
  switch(vertex_type_get(&detail->vertex)) {
    case VERTEX_FORMULA_STDIN:
      /* not passing anything to formulas on stdin yet */
      return;
    case VERTEX_FORMULA_ARGS:
    case VERTEX_FORMULA_ENVS:
    case VERTEX_FORMULA_FILE:
    case VERTEX_FORMULA_CWD:
      return formula_tell(detail, handler);
    case VERTEX_FORMULA_STDOUT:
      adaptor.buf = rec->stdout_buf;
      adaptor.sz = rec->stdout_size;
      adaptor.tail = &rec->stdout_tail;
      adaptor.head = &rec->stdout_head;
      break;
    case VERTEX_FORMULA_STDERR:
      adaptor.buf = rec->stderr_buf;
      adaptor.sz = rec->stderr_size;
      adaptor.tail = &rec->stderr_tail;
      adaptor.head = &rec->stderr_head;
      break;
    case VERTEX_FORMULA_AUXOUT:
      adaptor.buf = rec->auxout_buf;
      adaptor.sz = rec->auxout_size;
      adaptor.tail = &rec->auxout_tail;
      adaptor.head = &rec->auxout_head;
      break;
    default:
      RTA;
  }

  if((len = adaptor_dump0(&adaptor, &buf))) {
    msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_TEXT, 0);
    msg->attrs = 0;
    RUNTIME_ASSERT(len < msg->size);
    memcpy(msg->text, buf, len);
    msg->len = len;
    request_handler_post(handler, msg);
  }
  if((len = adaptor_dump1(&adaptor, &buf))) {
    msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_TEXT, 0);
    msg->attrs = 0;
    RUNTIME_ASSERT(len < msg->size);
    memcpy(msg->text, buf, len);
    msg->len = len;
    request_handler_post(handler, msg);
  }
}

void formula_record_id(const struct formula_record * restrict formula_record, const char ** restrict id, uint16_t * restrict idl)
{
  *id = formula_record->id;
  *idl = formula_record->id_len;
}

void formula_record_compute_id(const formula_record * restrict rec, identity_ctx * restrict id)
{
  vertex_compute_id(&rec->build_rec->vertex, id);
  vertex_compute_id(&rec->bpe->vertex, id);
  identity_type(id, VERTEX_FORMULA_RECORD);
}

#if CONFIG_DEVEL
void formula_records_show(void)
{
  struct formula_record *rec;
  int x = 0;

  printf("fml-recs, live : %zu free : %zu\n", llist_count(&formula_record_list), llist_count(&freelist));
  llist_foreach(&formula_record_list, rec, vertex.owner) {
    printf("%3u ", x++);
    vertex_identity_show(&rec->vertex);
  }
}
#endif
