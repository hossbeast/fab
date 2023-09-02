/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "moria/vertex.h"
#include "moria/foreach.h"
#include "bacon/expr.h"
#include "bacon/parser.h"
#include "valyria/llist.h"

#include "block.h"
#include "graph.h"
#include "pattern.h"
#include "rule.h"
#include "import.h"
#include "edge.h"
#include "entlist.h"

#include "macros.h"

static llist block_freelist = LLIST_INITIALIZER(block_freelist);  // freelist

static void block_dispose(bacon_parser * restrict parser, block * restrict block)
{
  rule *rule;
  struct block *sb;
  struct import *import;
  llist *T;
  bacon_expr *expr;

  llist_foreach_safe(&block->exprs, expr, lln, T) {
    bacon_expr_free(parser, expr);
  }
  llist_foreach_safe(&block->blocks, sb, lln, T) {
    block_dispose(parser, sb);
  }
  llist_foreach_safe(&block->rules, rule, lln, T) {
    rule_dispose(rule);
  }
//  llist_foreach_safe(&block->binds, bind, lln, T) {
//    bind_dispose(bind);
//  }
  llist_foreach_safe(&block->imports, import, lln, T) {
    import_dispose(import);
  }

  llist_delete(&block->lln);
  llist_append(&block_freelist, &block->lln);
}

void block_release(bacon_parser * restrict parser, block * restrict block)
{
  rule *rule;
  struct block *sb;
  struct import *import;
  llist *T;
  bacon_expr *expr;

  llist_foreach_safe(&block->exprs, expr, lln, T) {
    bacon_expr_free(parser, expr);
  }
  llist_foreach_safe(&block->blocks, sb, lln, T) {
    block_release(parser, sb);
  }
  llist_foreach_safe(&block->rules, rule, lln, T) {
    rule_release(rule);
  }
  llist_foreach_safe(&block->imports, import, lln, T) {
    import_release(import);
  }

  llist_delete(&block->lln);
  llist_append(&block_freelist, &block->lln);
}

void block_init(block * restrict b)
{
  llist_init(&b->lln);
  llist_init(&b->rules);
  llist_init(&b->imports);
  llist_init(&b->blocks);
  llist_init(&b->exprs);
}

void block_alloc(block ** restrict blockp)
{
  block *r;

  if((r = llist_shift(&block_freelist, typeof(*r), lln)) == 0) {
    xmalloc(&r, sizeof(*r));
  }

  block_init(r);

  *blockp = r;
}

void block_release_bulk(bacon_parser * restrict parser, llist * restrict list)
{
  block *block;

  while((block = llist_shift(list, typeof(*block), lln))) {
    block_release(parser, block);
  }
}

void block_dispose_bulk(bacon_parser * restrict parser, llist * restrict list)
{
  block *block;

  while((block = llist_shift(list, typeof(*block), lln))) {
    block_dispose(parser, block);
  }
}

void block_dump(block * restrict b)
{
  int x;
  block *sb;
  rule *rule;

size_t rc = llist_count(&b->rules);
size_t cc = llist_count(&b->blocks);
  printf(" block %p -- rules %2zu blocks %2zu\n", b, rc, cc);

  x = 0;
  llist_foreach(&b->blocks, sb, lln) {
    printf("  %2u ", x);
    block_dump(sb);
    x++;
  }

  x = 0;
  llist_foreach(&b->rules, rule, lln) {
    printf("  %2u ", x);
    rule_say(rule, g_narrator_stdout);
    printf("\n");
    x++;
  }
}

void blocks_dump(llist * restrict list)
{
  int x;
  block *b;

  x = 0;
  printf("stack %p has %zu blocks\n", list, llist_count(list));
  llist_foreach(list, b, lln) {
    printf(" %2u ", x);
    block_dump(b);
    x++;
  }
}

void block_cleanup(void)
{
  block * block;

  while((block = llist_shift(&block_freelist, typeof(*block), lln))) {
    free(block);
  }
}

void block_gather_rule_edges(entlist * restrict list, block * restrict block)
{
  rule *rule;
  struct block *sb;
  moria_edge *edge;
  moria_foreach_state st;

  llist_foreach(&block->blocks, sb, lln) {
    block_gather_rule_edges(list, sb);
  }

  llist_foreach(&block->rules, rule, lln) {
    moria_foreach_edge_up(&st, &rule->vertex, edge) {
      RUNTIME_ASSERT(edge_type_get(edge)== EDGE_RULE_RMA);
      entlist_push(list, edge);
    }
  }
}
