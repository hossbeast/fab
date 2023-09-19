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


#include "common/attrs.h"
#include "moria/edge.h"
#include "moria/operations.h"
#include "value/parser.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"

#include "narrator.h"

/* flex and bison do not agree on these names */
#define YYSTYPE MODULE_YYSTYPE
#define YYLTYPE yyu_location

#include "module_parser.internal.h"
#include "module.tab.h"
#include "module.tokens.h"
#include "module.lex.h"
#include "module.states.h"

#include "channel.h"
#include "fsedge.h"
#include "fsent.h"
#include "graph.h"
#include "lookup.h"
#include "pattern_parser.h"
#include "render.h"
#include "selection.h"
#include "shadow.h"

static YYU_VTABLE(vtable, module, module);

//
// static
//

/* module A imports directory B */
static void resolve_import(module_parser * restrict parser, pattern * restrict ref, bool scoped, char * restrict name, uint16_t namel)
{
  moria_vertex *referent;
  fsent * refnode;
  selected *sn;
  module *mod;
  const char * refname;
  uint16_t refnamel;

  mod = parser->mod;

  selection_reset(parser->scratch, SELECTION_ITERATION_TYPE_ORDER);
  pattern_lookup(ref, PATTERN_LOOKUP_DIR, parser->scratch, 0);
  selection_finalize(parser->scratch);

  llist_foreach(&parser->scratch->list, sn, lln) {
    RUNTIME_ASSERT((sn->v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_FSENT);

    // referent is directory node B
    referent = sn->v;
    refnode = containerof(referent, fsent, vertex);
    if(name)
    {
      refname = name;
      refnamel = namel;
    }
    else
    {
      refname = referent->label;
      refnamel = referent->label_len;
    }

    module_resolve_import(mod, refnode, refname, refnamel, scoped, parser->invalidation);
  }

  pattern_free(ref);
  free(name);
}

/* module A uses model B */
static void resolve_use(module_parser * restrict parser, pattern * restrict ref, bool scoped, char * restrict name, uint16_t namel)
{
  fsent * mod_file_n;
  moria_vertex * mod_file_v;
  module *mod;
  selected *sn;
  const char *refname;
  uint16_t refnamel;
  moria_vertex *referent;
  fsent * refnode;

  mod = parser->mod;

  selection_reset(parser->scratch, SELECTION_ITERATION_TYPE_ORDER);
  pattern_lookup(ref, PATTERN_LOOKUP_MODEL, parser->scratch, 0);
  selection_finalize(parser->scratch);

  llist_foreach(&parser->scratch->list, sn, lln) {
    RUNTIME_ASSERT((sn->v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_FSENT);

    // referent is the directory node
    referent = sn->v;
    refnode = containerof(referent, fsent, vertex);

    mod_file_v = moria_vertex_downw(referent, MMS(fsent_name_model));
    mod_file_n = containerof(mod_file_v, fsent, vertex);
    module_bootstrap(refnode, mod_file_n, parser->invalidation);

    if(name)
    {
      refname = name;
      refnamel = namel;
    }
    else
    {
      refname = referent->label;
      refnamel = referent->label_len;
    }

    module_resolve_use(mod, refnode->mod, refname, refnamel, scoped, parser->invalidation);
  }

  pattern_free(ref);
  free(name);
}

/* module A requires module B */
static void resolve_require(module_parser * restrict parser, pattern * restrict ref)
{
  fsent * mod_dir_n;
  fsent * mod_file_n;
  moria_vertex * mod_file_v;
  moria_vertex * mod_dir_v;
  module *mod;
  selected *sn;

  mod = parser->mod;

  selection_reset(parser->scratch, SELECTION_ITERATION_TYPE_ORDER);
  pattern_lookup(ref, PATTERN_LOOKUP_MODULE, parser->scratch, 0);
  selection_finalize(parser->scratch);

  llist_foreach(&parser->scratch->list, sn, lln) {
    RUNTIME_ASSERT((sn->v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_FSENT);

    mod_dir_v = sn->v;
    mod_dir_n = containerof(mod_dir_v, fsent, vertex);
    mod_file_v = moria_vertex_downw(mod_dir_v, MMS(fsent_name_module));
    mod_file_n = containerof(mod_file_v, fsent, vertex);
    module_bootstrap(mod_dir_n, mod_file_n, parser->invalidation);
    module_resolve_require(mod, mod_dir_n->mod, parser->invalidation);
  }

  pattern_free(ref);
}

//
// internal
//

void module_parser_block_alloc(module_parser * restrict parser, statement_block ** restrict blockp)
{
  statement_block *block;

  if((block = llist_shift(&parser->statement_block_freelist, typeof(*block), lln)) == 0)
  {
    xmalloc(&block, sizeof(*block));
    set_create(&block->variants);
  }
  else
  {
    set_recycle(block->variants);
  }

  llist_init_node(&block->rules);

  *blockp = block;
}

//
// public
//

void module_parser_create(module_parser ** rv)
{
  module_parser * p = 0;

  xmalloc(&p, sizeof(*p));

  yyu_parser_init(&p->yyu, &vtable);
  yyu_parser_init_tokens(&p->yyu, module_token_table, module_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->yyu
    , module_numstates
    , module_statenumbers
    , module_statenames
  );

  pattern_parser_create(&p->pattern_parser);
  set_create(&p->variants);
  p->g = &g_graph;
  selection_create(&p->scratch, 0);

  // callbacks
  p->require_resolve = resolve_require;
  p->use_resolve = resolve_use;
  p->import_resolve = resolve_import;

  llist_init_node(&p->statement_block_freelist);
  llist_init_node(&p->scoped_blocks);

  *rv = p;
  p = 0;

  module_parser_xfree(p);
}

void module_parser_xfree(module_parser* const p)
{
  statement_block *block;
  llist *cursor;

  if(p)
  {
    yyu_parser_xdestroy(&p->yyu);
    pattern_parser_xfree(p->pattern_parser);
    set_xfree(p->variants);
    selection_xfree(p->scratch);

    if(p->unscoped_block) {
      llist_append(&p->statement_block_freelist, p->unscoped_block, lln);
    }
    if(p->scoped_block) {
      llist_append(&p->statement_block_freelist, p->scoped_block, lln);
    }
    llist_splice_head(&p->statement_block_freelist, &p->scoped_blocks);

    llist_foreach_safe(&p->statement_block_freelist, block, lln, cursor) {
      statement_block_xdestroy(block);
      wfree(block);
    }
  }

  wfree(p);
}

void module_parser_ixfree(module_parser ** const p)
{
  module_parser_xfree(*p);
  *p = 0;
}

int module_parser_parse(
    module_parser * restrict parser
  , module * restrict mod
  , graph_invalidation_context * restrict invalidation
  , char * const restrict buf
  , size_t size
  , const char * fname
)
{
  set_recycle(parser->variants);

  parser->mod = mod;
  parser->invalidation = invalidation;
  parser->value_parser = mod->value_parser;

  return yyu_parse(&parser->yyu, buf, size, fname, YYU_INPLACE, 0, 0);
}
