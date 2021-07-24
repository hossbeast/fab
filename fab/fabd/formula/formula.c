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

#include <errno.h>

#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"

#include "formula.h"
#include "formula_parser.h"
#include "formula_value.h"
#include "fsent.h"
#include "stats.h"
#include "marshal.h"
#include "channel.h"
#include "system_state.h"
#include "events.h"
#include "formula_parser.internal.h"

#include "common/snarf.h"
#include "zbuffer.h"

static llist formula_list = LLIST_INITIALIZER(formula_list);
static llist formula_freelist = LLIST_INITIALIZER(formula_freelist);

static formula_parser *parser;

static void formula_parse(formula * restrict fml)
{
  char * text = 0;
  size_t text_len;
  int exit;
  channel *chan;
  fabipc_message *msg;

  // open the file, both to read its contents, and to exec later
  ixclose(&fml->fd);
  fml->fd = xopenats(O_RDONLY, 0, fml->self_node_abspath);

  formula_value_ifree(&fml->file);
  formula_value_ifree(&fml->envs);
  formula_value_ifree(&fml->args);

  STATS_INC(fml->stats.parsed_try);
  STATS_INC(g_stats.formula_parsed_try);

  fsnarf(&text, &text_len, fml->fd);

  if(text_len)
  {
    if((exit = formula_parser_parse(parser, text, text_len, fml->self_node_abspath, fml)))
    {
      system_error = true;
      if(!events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
        goto end;
      }

      msg->code = EINVAL;
      msg->size = znloadw(msg->text, sizeof(msg->text), parser->yyu.error_str, parser->yyu.error_len);
      events_publish(chan, msg);

      goto end;
    }
  }

  STATS_INC(fml->stats.parsed);
  STATS_INC(g_stats.formula_parsed);

  //logf(L_MODULE, "parsed formula @ %s via fd %d", fml->self_node_abspath, fml->fd);

end:
  wfree(text);
}

static void formula_dispose(formula * restrict fml)
{
  xclose(fml->fd);
  formula_value_free(fml->file);
  formula_value_free(fml->envs);
  formula_value_free(fml->args);

  llist_delete_node(&fml->vertex.owner);
  llist_append(&formula_freelist, fml, vertex.owner);
}

static void formula_xrelease(formula * restrict fml)
{
  moria_vertex __attribute__((unused)) *v;

  /* should have been fully excised from the graph previously */
  v = &fml->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  formula_dispose(fml);
}

//
// public
//

void formula_setup()
{
  formula_parser_create(&parser);
}

void formula_cleanup()
{
  formula *fml;
  llist *T;

  formula_parser_xfree(parser);

  llist_foreach_safe(&formula_list, fml, vertex.owner, T) {
    formula_dispose(fml);
  }

  llist_foreach_safe(&formula_freelist, fml, vertex.owner, T) {
    free(fml);
  }
}

void formula_reconcile(formula * restrict fml)
{
  if(!fsent_invalid_get(fml->self_node)) {
    return;
  }

  formula_parse(fml);

  if(!system_error) {
    fsent_ok(fml->self_node);
  }
}

void formula_system_reconcile()
{
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
    graph_disconnect(e);
  }

  /* release orphaned formulas */
  llist_foreach_safe(&formula_list, fml, vertex.owner, T) {
    if(fml->refs != 0) {
      continue;
    }

    /* un-bless the vertex */
    RUNTIME_ASSERT(fsent_kind_get(fml->self_node) == VERTEX_FORMULA_FILE);
    fsent_kind_set(fml->self_node, VERTEX_FILE);

    formula_xrelease(fml);
  }
}

void formula_create(formula ** restrict fmlp, moria_graph * restrict g)
{
  formula *fml;

  if((fml = llist_shift(&formula_freelist, typeof(*fml), vertex.owner)) == 0)
  {
    xmalloc(&fml, sizeof(*fml));
  }

  moria_vertex_init(&fml->vertex, g, VERTEX_FML);
  llist_append(&formula_list, fml, vertex.owner);

  fml->fd = -1;

  *fmlp = fml;
}

void formula_collate_stats(void *dst, size_t sz, formula *fml, bool reset, size_t *zp)
{
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
}
