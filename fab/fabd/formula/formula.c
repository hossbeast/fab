/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"

#include "formula.h"
#include "formula_parser.h"
#include "formula_value.h"
#include "logging.h"
#include "fsent.h"
#include "stats.h"
#include "marshal.h"

#include "common/snarf.h"

static llist formula_list = LLIST_INITIALIZER(formula_list);
static llist formula_freelist = LLIST_INITIALIZER(formula_freelist);

static formula_parser *parser;

static xapi formula_parse(formula * restrict fml, bool * restrict success)
{
  enter;

  char * text = 0;
  size_t text_len;
  xapi exit;
  char trace[4096 << 1];

  // open the file, both to read its contents, and to exec later
  fatal(ixclose, &fml->fd);
  fatal(xopenats, &fml->fd, O_RDONLY, 0, fml->self_node_abspath);

  formula_value_ifree(&fml->file);
  formula_value_ifree(&fml->envs);
  formula_value_ifree(&fml->args);

  STATS_INC(fml->stats.parsed_try);
  STATS_INC(g_stats.formula_parsed_try);

  fatal(fsnarf, &text, &text_len, fml->fd);

  if(text_len)
  {
    if((exit = invoke(formula_parser_parse, parser, text, text_len, fml->self_node_abspath, fml)))
    {
#if DEBUG || DEVEL || XAPI
      xapi_trace_full(trace, sizeof(trace), 0);
#else
      xapi_trace_pithy(trace, sizeof(trace), 0);
#endif
      xapi_calltree_unwind();

      xlogs(L_WARN, L_NOCATEGORY, trace);

      *success = false;
      goto XAPI_FINALLY;
    }
  }

  STATS_INC(fml->stats.parsed);
  STATS_INC(g_stats.formula_parsed);

  logf(L_MODULE, "parsed formula @ %s via fd %d", fml->self_node_abspath, fml->fd);

finally:
  wfree(text);
  xapi_infos("path", fml->self_node_abspath);
coda;
}

static xapi formula_dispose(formula * restrict fml)
{
  enter;

  fatal(xclose, fml->fd);
  formula_value_free(fml->file);
  formula_value_free(fml->envs);
  formula_value_free(fml->args);

  llist_delete_node(&fml->vertex.owner);
  llist_append(&formula_freelist, fml, vertex.owner);

  finally : coda;
}

static xapi formula_xrelease(formula * restrict fml)
{
  enter;

  moria_vertex __attribute__((unused)) *v;

  /* should have been fully excised from the graph previously */
  v = &fml->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  fatal(formula_dispose, fml);

  finally : coda;
}

//
// public
//

xapi formula_setup()
{
  enter;

  fatal(formula_parser_create, &parser);

  finally : coda;
}

xapi formula_cleanup()
{
  enter;

  formula *fml;
  llist *T;

  fatal(formula_parser_xfree, parser);

  llist_foreach_safe(&formula_list, fml, vertex.owner, T) {
    fatal(formula_dispose, fml);
  }

  llist_foreach_safe(&formula_freelist, fml, vertex.owner, T) {
    free(fml);
  }

  finally : coda;
}

xapi formula_reconcile(formula * restrict fml, bool * restrict reconciled)
{
  enter;

  if(!fsent_invalid_get(fml->self_node)) {
    goto XAPI_FINALLY;
  }

  fatal(formula_parse, fml, reconciled);

  if(*reconciled) {
    fatal(fsent_ok, fml->self_node);
  }

  finally : coda;
}

xapi formula_system_reconcile(bool * restrict reconciled)
{
  enter;

  formula *fml;
  llist *T;
  moria_edge *e;
  llist lln;

  /* collect edges for orphaned formulas */
  llist_init_node(&lln);
  llist_foreach(&formula_list, fml, vertex.owner) {
    if(fml->refs != 0) {
      continue;
    }

    rbtree_foreach(&fml->vertex.up, e, rbn_up) {
      RUNTIME_ASSERT(e->attrs == EDGE_RULE_FML);
      llist_append(&lln, e, lln);
    }
  }

  llist_foreach_safe(&lln, e, lln, T) {
    fatal(graph_disconnect, e);
  }

  /* release orphaned formulas */
  llist_foreach_safe(&formula_list, fml, vertex.owner, T) {
    if(fml->refs != 0) {
      continue;
    }

    /* un-bless the vertex */
    RUNTIME_ASSERT(fsent_kind_get(fml->self_node) == VERTEX_FORMULA_FILE);
    fsent_kind_set(fml->self_node, VERTEX_FILE);

    fatal(formula_xrelease, fml);
  }

  finally : coda;
}

xapi formula_create(formula ** restrict fmlp, moria_graph * restrict g)
{
  enter;

  formula *fml;

  if((fml = llist_shift(&formula_freelist, typeof(*fml), vertex.owner)) == 0)
  {
    fatal(xmalloc, &fml, sizeof(*fml));
  }

  moria_vertex_init(&fml->vertex, g, VERTEX_FML);
  llist_append(&formula_list, fml, vertex.owner);

  fml->fd = -1;

  *fmlp = fml;

  finally : coda;
}

xapi formula_collate_stats(void *dst, size_t sz, formula *fml, bool reset, size_t *zp)
{
  enter;

  size_t z;
  fab_formula_stats *stats;
  fab_formula_stats lstats;
  descriptor_field *field;
  int x;

  stats = &fml->stats;
  if(reset)
  {
    memcpy(&lstats, &stats, sizeof(lstats));
    memset(stats, 0, sizeof(*stats));
    stats = &lstats;
  }

  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_fab_formula_stats.id);

  /* event counters */
  for(x = 0; x < descriptor_fab_formula_stats.members_len; x++)
  {
    field = descriptor_fab_formula_stats.members[x];

    if(field->size == 8) {
      z += marshal_u64(dst + z, sz - z, stats->u64[field->offset / 8]);
    } else if(field->size == 4) {
      z += marshal_u32(dst + z, sz - z, stats->u32[field->offset / 4]);
    } else if(field->size == 2) {
      z += marshal_u16(dst + z, sz - z, stats->u16[field->offset / 2]);
    } else if(field->size == 1) {
      z += marshal_u16(dst + z, sz - z, stats->u8[field->offset / 1]);
    } else {
      RUNTIME_ABORT();
    }
  }

  *zp += z;

  finally : coda;
}
