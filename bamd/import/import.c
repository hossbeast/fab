/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include "types.h"

#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "moria/vertex.h"
#include "common/attrs.h"
#include "bacon/builder.h"

#include "import.h"
#include "graph.h"
#include "pattern.h"
#include "module.h"
#include "rule.h"
#include "fsent.h"
#include "channel.h"
#include "lookup.h"
#include "system.h"
#include "entlist.h"
#include "selection.h"
#include "events.h"
#include "generate.h"
#include "vertex.h"
#include "fsname.h"
#include "meta_edge.h"
#include "rule_system.h"

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
  , pattern *lookup
  , char * restrict name
  , uint16_t name_len
  , uint32_t attrs
)
{
  import * r;

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
  , pattern * lookup
  , char * restrict name
  , uint16_t name_len
  , uint32_t attrs
)
{
  import_alloc(importp, lookup, name, name_len, attrs | IMPORT_TYPE_USES);
}

void import_requires_mk(
    import ** restrict importp
  , pattern * lookup
  , char * restrict name
  , uint16_t name_len
  , uint32_t attrs
)
{
  import_alloc(importp, lookup, name, name_len, attrs | IMPORT_TYPE_REQUIRES);
}

void import_dispose(import * restrict import)
{
  free(import->name);
  pattern_free(import->lookup);

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

  pattern_say(b->lookup, N);
  if(b->attrs & IMPORT_FLAGS_NONAME) {
    xsays(" as");
  } else if(b->name) {
    xsays(" as ");
    xsayw(b->name, b->name_len);
  }
}

void import_resolve_targets(import * restrict import, module * restrict mod, rule_run_context * restrict ctx)
{
  moria_vertex *v;
  moria_vertex *target;
  const char *target_file_name;
  events_context ectx;
  event_bacon *msg;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  const yyu_location *loc;
  entlist list, *eln;
  fsname *index;
  meta_edge *me;

  /* lookup the import target node */
  entlist_destroy(&ctx->target_hits);
  pattern_lookup_indexed(import->lookup, mod->vars, &ctx->target_index_hits, &ctx->target_hits);
  if(system_has_error) {
    return;
  }

  /* take an edge fsname nodes visited during targets resolution */
  entlist_foreach(&ctx->target_index_hits, index, eln) {
    meta_connect(&index->vertex, &mod->vertex, EDGE_FSNAME_MOD, &ctx->invalidation, &me);
    me->epoch = rule_system_epoch;
  }

  /* for a import-type import, the matching nodes are the result set */
  /* for a use or requires-type import, the matching nodes are the named files in that directory */
  switch(import->type) {
    case IMPORT_TYPE_USES:
      target_file_name = fsent_name_rules;
      break;
    case IMPORT_TYPE_REQUIRES:
      target_file_name = fsent_name_module;
      break;
    default:
      RTA;
  }

  entlist_init(&list);
  entlist_foreach(&ctx->target_hits, v, eln) {
    entlist_push(&list, v);
  }

  entlist_destroy(&ctx->target_hits);
  entlist_foreach(&list, v, eln) {
    if(!(target = moria_vertex_downw(v, MMS(target_file_name)))) {
      /* directory found, but no module file is present */
      msg = system_error_event(&ectx);
      narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
      bacon_writer_init(&writer);
      builder_writer = bacon_writer_open(&writer, &fixed.base);

      { MAPPING
        LOADF("unresolved %s ref", target_file_name)
        { SET
          { MAPPING
            STRING("import")
            bacon_writer_push_string(builder_writer);
            import_say(import, builder_writer->N);
            loc = &import->lookup->loc;
            LOADF(" @ %s [%d,%d - %d,%d]", import->lookup->fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);
            bacon_writer_pop_string(builder_writer);
          }
          { MAPPING
            STRING("module")
            fsent_absolute_path_describe(mod->dir_node, builder_writer);
          }
          { MAPPING
            STRING("target")
            loc = &import->lookup->loc;
            LOADF("%s @ %s [%d,%d - %d,%d]", target_file_name, import->lookup->fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);
          }
        }
      }
      bacon_writer_close(&writer);

      msg->size = fixed.l;
      msg->code = EINVAL;
      events_publish(&ectx);
      break;
    }
    entlist_push(&ctx->target_hits, target);
  }

  entlist_destroy(&list);
}
