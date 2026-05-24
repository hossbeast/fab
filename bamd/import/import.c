/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include "types.h"

#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"

#include "import.h"
#include "pattern.h"
#include "rule.h"
#include "fsent.h"
#include "system.h"
#include "edges.h"
#include "vertices.h"
#include "events/events.h"
#include "vertex.h"
#include "fsname.h"
#include "meta_edge.h"
#include "rule_system.h"
#include "namespace.h"

#include "macros.h"

attrs32 import_type_attrs = (attrs32) {
#undef DEF
#define DEF(x, s, y) + 1
    num : 0
      IMPORT_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, y) { name : s, value : UINT32_C(y), range : IMPORT_TYPE_OPT },
IMPORT_TYPE_TABLE
#undef DEF
  }
};

llist import_list = LLIST_INITIALIZER(import_list);                 // active import
static llist import_freelist = LLIST_INITIALIZER(import_freelist);  // freelist

static void __attribute__((constructor)) init()
{
  attrs32_init(&import_type_attrs);
}

static void import_alloc(
    import ** restrict importp
  , lookup_pattern *lookup
  , char * restrict name
  , uint16_t name_len
  , uint32_t attrs
)
{
  import *r;

  if((r = llist_shift(&import_freelist, typeof(*r), owner)) == 0) {
    xmalloc(&r, sizeof(*r));
  }

  llist_append(&import_list, &r->owner);

  r->lookup = lookup;
  r->attrs = attrs;
  r->name = name;
  r->name_len = name_len;

  *importp = r;
}

void import_uses_mk(
    import ** restrict importp
  , lookup_pattern * lookup
  , char * restrict name
  , uint16_t name_len
  , uint32_t attrs
)
{
  import_alloc(importp, lookup, name, name_len, attrs | IMPORT_TYPE_USES);
}

void import_dispose(import * restrict import)
{
  free(import->name);
  pattern_free(&import->lookup->pattern);

  llist_delete(&import->owner);
  llist_push(&import_freelist, &import->owner);
}

void import_release(import * restrict import)
{
  import_dispose(import);
}

void import_say(const import * restrict b, narrator * restrict N)
{
  xsays(attrs32_name_byvalue(&import_type_attrs, b->type));
  xsays(" ");

  pattern_say(&b->lookup->pattern, N);
  if(b->attrs & IMPORT_FLAGS_NONAME) {
    xsays(" as");
  } else if(b->name) {
    xsays(" as ");
    xsayw(b->name, b->name_len);
  }
}

static void namespace_resolve_error(struct lookup_context * restrict ctx, import_type type, int r)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    if(r == 0) {
      if(type == IMPORT_TYPE_USES) {
        STRING("unresolved use ref");
      } else {
        STRING("unresolved require ref");
      }
    } else {
      if(type == IMPORT_TYPE_USES) {
        STRING("ambiguous use ref");
      } else {
        STRING("ambiguous require ref");
      }
    }
    lookup_namespace_error_build(builder_writer, ctx);
  }

  bacon_writer_close(&writer);
  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void no_module_error(import * restrict import, module * restrict mod)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;
  const char *target_file_name;
  const yyu_location *loc;

  /* directory found, but no module file is present */
  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  switch(import->type) {
    case IMPORT_TYPE_USES:
      target_file_name = FSENT_NAME_RULESET;
      break;
    default:
      RTA;
  }

  { MAPPING
    LOADF("unresolved %s ref", target_file_name)
    { SET
      { MAPPING
        STRING("import")
        bacon_writer_push_string(builder_writer);
        import_say(import, builder_writer->N);
        loc = &import->lookup->pattern.loc;
        LOADF(" @ %s [%d,%d - %d,%d]", import->lookup->pattern.fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);
        bacon_writer_pop_string(builder_writer);
      }
      { MAPPING STRING("module")
        fsent_absolute_path_describe(mod->build_dir, builder_writer);
      }
      { MAPPING STRING("target")
        loc = &import->lookup->pattern.loc;
        LOADF("%s @ %s [%d,%d - %d,%d]", target_file_name, import->lookup->pattern.fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);
      }
    }
  }
  bacon_writer_close(&writer);

  msg->len = fixed.l;
  msg->code = EINVAL;
  events_publish(&ectx);
}

fsent *import_resolve(
    struct import * restrict import
  , module * restrict mod
  , rule_run_context * restrict ctx)
{
  moria_vertex *target;
  const char *target_file_name;
  meta_edge *me;
  int r;
  struct lookup_context *lctx;
  struct namespace *ns;

  /* modules is the default namespace for import resolution */
  RUNTIME_ASSERT(namespace_modules);
  ns = namespace_modules;

  /* lookup the target from the import */
  lctx = &ctx->lctx;
  memset(lctx, 0, sizeof(*lctx));
  lctx->ns = ns;
  lctx->ns_name = ns->name;
  lctx->ns_name_len = ns->name_len;
  lctx->rctx.vars = mod->vars;
  lctx->loc = &import->lookup->pattern.loc;
  if((r = namespace_resolve(lctx, import->lookup)) != 1) {
    namespace_resolve_error(lctx, import->type, r);
  }

  /* take an edge on the namespace used */

  /* take an edge the fsname visited */
  if(lctx->hit) {
    me = meta_connect(&lctx->hit->vertex, &mod->vertex, EDGE_FSNAME_MOD, &ctx->invalidation);
    me->epoch = rule_system_epoch;
  }
  if(lctx->miss) {
    me = meta_connect(&lctx->miss->vertex, &mod->vertex, EDGE_FSNAME_MOD, &ctx->invalidation);
    me->epoch = rule_system_epoch;
  }

  if(system_has_error) {
    return 0;
  }

  /* for a import-type import, the matching nodes are the result set */
  /* for a use or requires-type import, the matching nodes are the named files in that directory */
  switch(import->type) {
    case IMPORT_TYPE_USES:
      target_file_name = FSENT_NAME_RULESET;
      break;
    default:
      RTA;
  }

  if(!(target = moria_vertex_downw(lctx->rctx.vertices[0], MMS(target_file_name)))) {
    no_module_error(import, mod);
    return 0;
  }

  return containerof(target, struct fsent, vertex);
}
