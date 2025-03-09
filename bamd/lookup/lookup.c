/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <inttypes.h>

#include "types.h"

#include "xlinux/xstdlib.h"
#include "bacon/value.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "bacon/location.h"
#include "narrator/fixed.h"

#include "lookup.h"
#include "fsent.h"
#include "pattern.h"
#include "events.h"
#include "system.h"
#include "resolve.h"
#include "namespace.h"
#include "meta_edge.h"
#include "fsname.h"

static llist lookup_entry_freelist = LLIST_INITIALIZER(lookup_entry_freelist);

static int lookup_entry_key_cmp(void * restrict key, const rbnode * restrict B)
{
  lookup_entry *b = containerof(B, typeof(*b), rbn);

  return INTCMP(key, b->key);
}

int lookup_resolve(lookup_context * restrict ctx)
{
  fsent *fse = 0;
  moria_vertex *v;

  if(ctx->ref->bound) {
    ctx->r = pattern_resolve_bound(ctx);
    return ctx->r;
  }

  if((ctx->ns = namespace_lookup(ctx->ns_name, ctx->ns_name_len)) == 0) {
    return ctx->r = 0;
  }

  if((ctx->r = namespace_resolve(ctx)) != 1) {
    return ctx->r;
  }
  v = ctx->vertices[0];
  fse = containerof(v, typeof(*fse), vertex);

  return ctx->r;
}

void lookup_namespace_error_build(bacon_writer * restrict builder_writer, const lookup_context * restrict ctx)
{
  { SET
    { MAPPING STRING("pattern")
      bacon_writer_push_bytes(builder_writer);
      pattern_say(ctx->ref, builder_writer->N);
      bacon_writer_pop_bytes(builder_writer);
    }
    { MAPPING STRING("location")
      LOADF("%s [%d,%d - %d,%d]", ctx->loc->fname, ctx->loc->f_lin + 1, ctx->loc->f_col + 1, ctx->loc->l_lin + 1, ctx->loc->l_col + 1);
    }
    MAPPING STRING("namespace")
    if(ctx->ns_name_len) {
      BYTES(ctx->ns_name, ctx->ns_name_len)
    } else if(ctx->ns) {
      BYTES(ctx->ns->name, ctx->ns->name_len)
    } else {
      STRING("_")
    }
    if(ctx->r > 1) {
      { MAPPING
        STRING("one")
        fsent_absolute_path_describe(containerof(ctx->vertices[0], fsent, vertex), builder_writer);
      }
      { MAPPING
        STRING("two")
        fsent_absolute_path_describe(containerof(ctx->vertices[1], fsent, vertex), builder_writer);
      }
    }
  }
}

void lookup_bound_error_build(bacon_writer * restrict builder_writer, const pattern * restrict ref)
{
  { SET
    { MAPPING STRING("pattern")
      bacon_writer_push_bytes(builder_writer);
      pattern_say(ref, builder_writer->N);
      bacon_writer_pop_bytes(builder_writer);
    }
    { MAPPING STRING("location")
      LOADF("%s [%d,%d - %d,%d]", ref->loc.fname, ref->loc.f_lin + 1, ref->loc.f_col + 1, ref->loc.l_lin + 1, ref->loc.l_col + 1);
    }
  }
}

void lookup_error_mappings(bacon_writer * restrict builder_writer, const lookup_context * restrict ctx)
{
  { MAPPING STRING("pattern")
    bacon_writer_push_bytes(builder_writer);
    pattern_say(ctx->ref, builder_writer->N);
    bacon_writer_pop_bytes(builder_writer);
  }
  { MAPPING STRING("location")
    LOADF("%s [%d,%d - %d,%d]", ctx->loc->fname, ctx->loc->f_lin + 1, ctx->loc->f_col + 1, ctx->loc->l_lin + 1, ctx->loc->l_col + 1);
  }
  if(!ctx->ref->bound) {
    if(ctx->ns) {
      MAPPING STRING("namespace")
      BYTES(ctx->ns->name, ctx->ns->name_len)
    } else {
      RUNTIME_ASSERT(ctx->ns_name_len);
      MAPPING STRING("namespace")
      BYTES(ctx->ns_name, ctx->ns_name_len)
    }
    if(ctx->r > 1) {
      { MAPPING STRING("one")
        fsent_absolute_path_describe(containerof(ctx->vertices[0], fsent, vertex), builder_writer);
      }
      { MAPPING STRING("two")
        fsent_absolute_path_describe(containerof(ctx->vertices[1], fsent, vertex), builder_writer);
      }
    }
  }
}

void lookup_error_build(bacon_writer * restrict builder_writer, const lookup_context * restrict ctx)
{
  { MAPPING
    if(ctx->ref->bound) {
      STRING("unmatched lookup")
    } else if(ctx->r > 1) {
      STRING("ambiguous lookup")
    } else {
      STRING("unresolved lookup")
    }
    { SET
      lookup_error_mappings(builder_writer, ctx);
    }
  }
}

void lookup_store_put(struct lookup_store * restrict store, void *key, const struct moria_vertex * restrict v)
{
  rbtree_search_context ctx;
  lookup_entry *entry;
  rbnode *rbn;

  if((rbn = rbtree_search(&store->set, &ctx, key, lookup_entry_key_cmp))) {
    entry = containerof(rbn, typeof(*entry), rbn);
    entry->v = (moria_vertex *)v;
    return;
  }

  if((entry = llist_shift(&lookup_entry_freelist, typeof(*entry), lln)) == 0) {
    xmalloc(&entry, sizeof(*entry));
  } else {
    memset(entry, 0, sizeof(*entry));
  }

  rbnode_init(&entry->rbn);
  entry->key = key;
  entry->v = (moria_vertex *)v;
  rbtree_insert(&store->set, &ctx, &entry->rbn);
}

moria_vertex *lookup_store_get(struct lookup_store * restrict store, void *key)
{
  rbnode *rbn;
  lookup_entry *entry;

  if((rbn = rbtree_lookup(&store->set, key, lookup_entry_key_cmp))) {
    entry = containerof(rbn, typeof(*entry), rbn);
    return entry->v;
  }

  return 0;
}

void lookup_store_init(lookup_store * restrict store)
{
  rbtree_init(&store->set);
}

void lookup_store_destroy(lookup_store * restrict store)
{
  lookup_entry *entry;

  rbtree_foreach(&store->set, entry, rbn) {
    llist_push(&lookup_entry_freelist, &entry->lln);
  }
}

void lookup_store_reset(lookup_store * restrict store)
{
  lookup_store_destroy(store);
  rbtree_init(&store->set);
}

void lookup_teardown()
{
  lookup_entry *entry;

  while((entry = llist_shift(&lookup_entry_freelist, typeof(*entry), lln))) {
    free(entry);
  }
}
