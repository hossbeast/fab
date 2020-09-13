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
#include "xapi/exit.h"

#include "xlinux/xstdlib.h"
#include "xlinux/KERNEL.errtab.h"
#include "valyria/set.h"
#include "value/parser.h"
#include "moria/edge.h"

#include "narrator.h"

/* flex and bison do not agree on these names */
#define YYSTYPE MODULE_YYSTYPE
#define YYLTYPE yyu_location

#include "module_parser.internal.h"
#include "errtab/MODULE.errtab.h"
#include "logging.h"
#include "module.tab.h"
#include "module.tokens.h"
#include "module.lex.h"
#include "module.states.h"

#include "node.h"
#include "node_operations.h"
#include "pattern_parser.h"
#include "graph.h"
#include "render.h"
#include "path.h"
#include "lookup.internal.h"
#include "selection.h"
#include "shadow.h"

#include "common/attrs.h"

static YYU_VTABLE(vtable, module, module);

//
// static
//

static xapi resolve_import(module_parser * restrict parser, pattern * restrict ref, bool scoped, char * restrict name, uint16_t namel)
{
  enter;

  char path[512];
  node * refnode;
  selected_node *sn;
  module *mod;
  edge *e, *iep, *sep;
  node_edge_imports *nei;
  const char * refname;
  uint16_t refnamel;

  mod = parser->mod;

  fatal(selection_reset, parser->scratch);
  fatal(pattern_lookup, ref, 0, 0, PATTERN_LOOKUP_DIR, parser->scratch, 0);
  fatal(selection_finalize, parser->scratch);

  llist_foreach(&parser->scratch->list, sn, lln) {
    refnode = sn->n;
    if(name)
    {
      refname = name;
      refnamel = namel;
    }
    else
    {
      refname = refnode->name->name;
      refnamel = refnode->name->namel;
    }

    iep = 0;
    sep = 0;
    if(scoped)
    {
      // attach to //module/imports/
      fatal(shadow_graft_imports, mod, refnode, refname, refnamel, &iep, parser->invalidation);

      // attach to (or override in) //module/scope/
      fatal(shadow_graft_scope, mod, refnode, refname, refnamel, false, &sep, parser->invalidation);
    }

    // allow traversal via the imports graph
    fatal(node_connect_generic, mod->dir_node, refnode, EDGE_TYPE_IMPORTS, parser->invalidation, &e);
    nei = edge_value(e);
    nei->shadow_epoch = node_shadow_epoch;
    nei->scope_edge = sep;
    nei->imports_edge = iep;

    if(log_would(L_MODULE))
    {
      node_path_znload(path, sizeof(path), refnode);
      logf(L_MODULE, " %s : imports %.*s -> %s", mod->dir_node->name->name, (int)refnamel, refname, path);
    }
  }

finally:
  pattern_free(ref);
  free(name);
coda;
}

static xapi resolve_use(module_parser * restrict parser, pattern * restrict ref)
{
  enter;

  char path[512];
  node * mod_dir_n;
  node * mod_file_n;
  vertex * mod_file_v;
  vertex * mod_dir_v;
  module *mod;
  selected_node *sn;

  mod = parser->mod;

  fatal(selection_reset, parser->scratch);
  fatal(pattern_lookup, ref, 0, 0, PATTERN_LOOKUP_MODEL, parser->scratch, 0);
  fatal(selection_finalize, parser->scratch);

  llist_foreach(&parser->scratch->list, sn, lln) {
    mod_dir_n = sn->n;
    mod_dir_v = vertex_containerof(mod_dir_n);
    mod_file_v = vertex_downs(mod_dir_v, NODE_MODEL_BAM);
    mod_file_n = vertex_value(mod_file_v);
    fatal(module_load, mod_dir_n, mod_file_n, parser->invalidation);
    fatal(node_connect_generic, mod->dir_node, mod_dir_n, EDGE_TYPE_USES, parser->invalidation, 0);

    if(log_would(L_MODULE))
    {
      node_path_znload(path, sizeof(path), mod_dir_n);
      logf(L_MODULE, " %s : uses %s -> %s", mod->dir_node->name->name, mod_dir_n->name->name, path);
    }
  }

finally:
  pattern_free(ref);
coda;
}

static xapi resolve_require(module_parser * restrict parser, pattern * restrict ref)
{
  enter;

  char path[512];
  node * mod_dir_n;
  node * mod_file_n;
  vertex * mod_file_v;
  vertex * mod_dir_v;
  module *mod;
  selected_node *sn;

  mod = parser->mod;

  fatal(selection_reset, parser->scratch);
  fatal(pattern_lookup, ref, 0, 0, PATTERN_LOOKUP_MODULE, parser->scratch, 0);
  fatal(selection_finalize, parser->scratch);

  llist_foreach(&parser->scratch->list, sn, lln) {
    mod_dir_n = sn->n;
    mod_dir_v = vertex_containerof(mod_dir_n);
    mod_file_v = vertex_downs(mod_dir_v, NODE_MODULE_BAM);
    mod_file_n = vertex_value(mod_file_v);
    fatal(module_load, mod_dir_n, mod_file_n, parser->invalidation);
    fatal(node_connect_generic, mod->dir_node, mod_dir_n, EDGE_TYPE_REQUIRES, parser->invalidation, 0);

    if(log_would(L_MODULE))
    {
      node_path_znload(path, sizeof(path), mod_dir_n);
      logf(L_MODULE, " %s : requires %s -> %s", mod->dir_node->name->name, mod_dir_n->name->name, path);
    }
  }

finally:
  pattern_free(ref);
coda;
}

static xapi resolve_formula(module_parser * restrict parser, pattern * restrict ref, struct node ** restrict target)
{
  enter;

  module *mod;

  /* note that a reference pattern can only render down to a single fragment */
  mod = parser->mod;
  fatal(pattern_lookup, ref, 0, mod->dir_node, 0, 0, target);

finally:
  pattern_free(ref);
coda;
}

//
// internal
//

xapi module_parser_block_alloc(module_parser * restrict parser, statement_block ** restrict blockp)
{
  enter;

  statement_block *block;

  if((block = llist_shift(&parser->statement_block_freelist, typeof(*block), lln)) == 0)
  {
    fatal(xmalloc, &block, sizeof(*block));
    fatal(set_create, &block->variants);
  }
  else
  {
    fatal(set_recycle, block->variants);
  }

  llist_init_node(&block->rules);

  *blockp = block;

  finally : coda;
}

//
// public
//

xapi module_parser_create(module_parser ** rv)
{
  enter;

  module_parser * p = 0;

  fatal(xmalloc, &p, sizeof(*p));

  fatal(yyu_parser_init, &p->yyu, &vtable, MODULE_SYNTAX);

  fatal(yyu_parser_init_tokens, &p->yyu, module_token_table, module_TOKEN_TABLE_SIZE);

  fatal(yyu_parser_init_states
    , &p->yyu
    , module_numstates
    , module_statenumbers
    , module_statenames
  );

#if DEBUG || DEVEL || XUNIT
  p->yyu.logs = L_MODULE;
#endif

//  fatal(yyu_define_tokenrange, &p->yyu, module_AS, module_VARIANT);

//  fatal(value_parser_create, &p->value_parser);
  fatal(pattern_parser_create, &p->pattern_parser);
  fatal(set_create, &p->variants);
  p->g = g_graph;
  fatal(selection_create, &p->scratch);

  // callbacks
  p->require_resolve = resolve_require;
  p->use_resolve = resolve_use;
  p->import_resolve = resolve_import;
  p->formula_resolve = resolve_formula;

  llist_init_node(&p->statement_block_freelist);
  llist_init_node(&p->scoped_blocks);

  *rv = p;
  p = 0;

finally:
  fatal(module_parser_xfree, p);
coda;
}

xapi module_parser_xfree(module_parser* const p)
{
  enter;

  statement_block *block;
  llist *cursor;

  if(p)
  {
    fatal(yyu_parser_xdestroy, &p->yyu);
    fatal(pattern_parser_xfree, p->pattern_parser);
    fatal(set_xfree, p->variants);
    fatal(selection_xfree, p->scratch);

    if(p->unscoped_block) {
      llist_append(&p->statement_block_freelist, p->unscoped_block, lln);
    }
    llist_splice_head(&p->statement_block_freelist, &p->scoped_blocks);

    llist_foreach_safe(&p->statement_block_freelist, block, lln, cursor) {
      fatal(statement_block_xdestroy, block);
      wfree(block);
    }
  }

  wfree(p);

  finally : coda;
}

xapi module_parser_ixfree(module_parser ** const p)
{
  enter;

  fatal(module_parser_xfree, *p);
  *p = 0;

  finally : coda;
}

xapi module_parser_parse(
    module_parser * restrict parser
  , module * restrict mod
  , graph_invalidation_context * restrict invalidation
  , char * const restrict buf
  , size_t size
  , const char * fname
)
{
  enter;

  fatal(set_recycle, parser->variants);

  parser->mod = mod;
  parser->invalidation = invalidation;
  parser->value_parser = mod->value_parser;
  fatal(yyu_parse, &parser->yyu, buf, size, fname, YYU_INPLACE, 0, 0);

  finally : coda;
}
