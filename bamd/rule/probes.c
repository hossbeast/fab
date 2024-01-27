/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "types.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "bacon/bacon_narrator.h"

#include "probes.h"
#include "rule.h"
#include "fsent.h"
#include "dependency.h"
#include "pattern.h"
#include "index_vertex.h"
#include "module.h"
#include "rule_module_association.h"
#include "buildplan_entity.h"
#include "vertex.h"

#include "usdt-sem.h"
#include "zcat.h"

#if 1
PROBE_SEM(rule, run) = 0;
PROBE_SEM(rule, connect) = 0;
PROBE_SEM(bind, attach) = 0;
PROBE_SEM(bind, connect) = 0;
#else
PROBE_SEM(rule, run) = 1;
PROBE_SEM(rule, connect) = 1;
PROBE_SEM(bind, attach) = 1;
PROBE_SEM(bind, connect) = 1;
#endif

void rule_run_probe(const rule_run_context * restrict ctx, const buildplan_entity * restrict bpe, bool b)
{
  narrator_growing growing;
  narrator *N;
  bacon_writer writer;
  bacon_writer *builder_writer;
  bacon_narrator bn;

  if(!rule_run_semaphore) {
    return;
  }

  N = narrator_growing_init(&growing);
  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  builder_writer = &writer;
  bacon_narrator_init(&bn);
  bacon_narrator_configure(&bn, N);

  { MAPPING
    STRING(__FUNCTION__)
    { SET
      { MAPPING
        STRING("epoch")
        UINT(rule_system_epoch)
      }
      vertex_describe(&ctx->rma->vertex, builder_writer);

      if(bpe) {
        MAPPING
        if(b) {
          STRING("bpe-create")
        } else {
          STRING("bpe-match")
        }
        { SET
          vertex_describe(&bpe->vertex, builder_writer);
        }
      } else {
        STRING("no-match")
      }
    }
  }

printf("%.*s\n", (int)growing.l, growing.s);
  PROBE1(rule, run, growing.s);
  bacon_narrator_destroy(&bn);
  bacon_writer_destroy(&writer);
  narrator_growing_destroy(&growing);
}

void rule_connect_probe(const rule * restrict rule, const buildplan_entity * restrict bpe)
{
  narrator_growing growing;
  narrator *N;
  bacon_writer writer;
  bacon_writer *builder_writer;
  bacon_narrator bn;

  if(!rule_connect_semaphore) {
    return;
  }

  N = narrator_growing_init(&growing);
  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  builder_writer = &writer;

  bacon_narrator_init(&bn);
  bacon_narrator_configure(&bn, N);

  { MAPPING
    STRING(__FUNCTION__)
    { SET
      { MAPPING
        STRING("rule")
        bacon_writer_push_string(builder_writer);
        rule_say(rule, &bn.base);
        bacon_narrator_flush(&bn);
        bacon_writer_pop_string(builder_writer);
      }

      vertex_describe(&bpe->vertex, builder_writer);
    }
  }

printf("%.*s\n", (int)growing.l, growing.s);
  PROBE1(rule, connect, growing.s);
  bacon_narrator_destroy(&bn);
  bacon_writer_destroy(&writer);
  narrator_growing_destroy(&growing);
}

void bind_run_probe(
    const struct rule * restrict rule
  , const module * restrict mod
  , const fsent * restrict attachment
  , const fsent * restrict location
  , const fsent * restrict target
)
{
  narrator_growing growing;
  narrator *N;
  bacon_writer writer;
  bacon_writer *builder_writer;
  const yyu_location *loc;

  if(!bind_attach_semaphore) {
    return;
  }

  N = narrator_growing_init(&growing);
  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  builder_writer = &writer;

  { MAPPING
    STRING(__FUNCTION__)
    { SET
      bacon_writer_push_string(builder_writer);
      rule_say(rule, builder_writer->N);
      loc = &rule->loc;
      LOADF(" @ %s [%d,%d - %d,%d]", loc->fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1)
      bacon_writer_pop_string(builder_writer);

      { MAPPING
        STRING("module")
        vertex_identity_describe(&mod->dir_node->vertex, builder_writer);
      }
      if(attachment) {
        MAPPING
        STRING("attachment")
        vertex_identity_describe(&attachment->vertex, builder_writer);
      }
      if(target) {
        { MAPPING
          STRING("target")
          vertex_identity_describe(&target->vertex, builder_writer);
        }
        { MAPPING
          STRING("location")
          vertex_identity_describe(&location->vertex, builder_writer);
        }
      } else {
        STRING("no-match")
      }
    }
  }

printf("%.*s\n", (int)growing.l, growing.s);
  PROBE1(bind, attach, growing.s);
  bacon_writer_destroy(&writer);
  narrator_growing_destroy(&growing);
}

void bind_connect_probe(
    const struct rule * restrict rule
  , const module * restrict mod
  , const entlist * restrict list
)
{
  narrator_growing growing;
  narrator *N;
  bacon_writer writer;
  bacon_writer *builder_writer;
  const yyu_location *loc;
  index_vertex *index;
  entlist *eln;

  if(!bind_attach_semaphore) {
    return;
  }

  N = narrator_growing_init(&growing);
  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  builder_writer = &writer;

  { MAPPING
    STRING(__FUNCTION__)
    { SET
      bacon_writer_push_string(builder_writer);
      rule_say(rule, builder_writer->N);
      loc = &rule->target->loc;
      LOADF(" @ %s [%d,%d - %d,%d]", rule->target->fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1)
      bacon_writer_pop_string(builder_writer);

      { MAPPING
        STRING("module")
        vertex_identity_describe(&mod->dir_node->vertex, builder_writer);
      }
      { LIST
        entlist_foreach(list, index, eln) {
          vertex_identity_describe(&index->vertex, builder_writer);
        }
      }
    }
  }

printf("%.*s\n", (int)growing.l, growing.s);
  PROBE1(bind, attach, growing.s);
  bacon_writer_destroy(&writer);
  narrator_growing_destroy(&growing);
}
