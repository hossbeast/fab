/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "xlinux/xstdlib.h"
#include "bacon/expr.h"
#include "bacon/parser.h"
#include "valyria/llist.h"

#include "block.h"
#include "rule.h"
#include "import.h"

static llist freelist = LLIST_INITIALIZER(freelist);

static void block_release(bacon_parser * restrict parser, struct block * restrict block)
{
  struct block *sb;
  struct import *import;
  //struct rule *rule;
  llist *T;
  bacon_expr *expr;

  //RUNTIME_ASSERT(llist_empty(&block->rules));

  llist_foreach_safe(&block->exprs, expr, lln, T) {
    bacon_expr_free(parser, expr);
  }
  llist_foreach_safe(&block->blocks, sb, lln, T) {
    block_release(parser, sb);
  }
  //llist_foreach_safe(&block->rules, rule, lln, T) {
  //  rule_release(rule);
  //}
  llist_foreach_safe(&block->imports, import, lln, T) {
    import_release(import);
  }

  llist_delete(&block->lln);
  llist_append(&freelist, &block->lln);
  ASAN_TAINT(block);
}

static void block_init(block * restrict b)
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

  if((r = llist_shift(&freelist, typeof(*r), lln))) {
    ASAN_CLEAN(r);
  } else {
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

void block_cleanup(void)
{
  block * block;

  while((block = llist_shift(&freelist, typeof(*block), lln))) {
    ASAN_CLEAN(block);
    free(block);
  }
}

void blocks_gather_rules(vertices * restrict list, llist * restrict blocks)
{
  struct rule *rule;
  struct block *sb;

  llist_foreach(blocks, sb, lln) {
    blocks_gather_rules(list, &sb->blocks);

    llist_foreach(&sb->rules, rule, lln) {
      vertices_push(list, &rule->vertex);
    }
  }
}
