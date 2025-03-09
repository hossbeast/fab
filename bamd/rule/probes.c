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
#include "fsname.h"
#include "module.h"
#include "rma.h"
#include "bpe.h"
#include "vertex.h"

#include "usdt-sem.h"
#include "zcat.h"

PROBE_SEM(rule, run) = 0;
PROBE_SEM(rule, connect) = 0;
PROBE_SEM(bind, attach) = 0;
//PROBE_SEM(bind, connect) = 1;

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
      { MAPPING STRING("epoch")
        UINT(rule_system_epoch)
      }
      { MAPPING STRING("rule")
        bacon_writer_push_string(builder_writer);
        rule_say(ctx->rma->rule, builder_writer->N);
        bacon_writer_pop_string(builder_writer);
      }
//      vertex_describe(&ctx->rma->vertex, builder_writer);

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
    const struct rule_module_association * restrict rma
  , const fsent * restrict attachment
  , const fsent * restrict target
  , const fsent * restrict binding
)
{
  narrator_growing growing;
  narrator *N;
  bacon_writer writer;
  bacon_writer *builder_writer;

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
      //bacon_writer_push_string(builder_writer);
      //rule_say(rma->rule, builder_writer->N);
      //loc = &rma->rule->loc;
      //LOADF(" @ %s [%d,%d - %d,%d]", loc->fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1)
      //bacon_writer_pop_string(builder_writer);

      { MAPPING STRING("rma")
        vertex_identity_describe(&rma->vertex, builder_writer);
      }
      { MAPPING STRING("attachment")
        if(attachment) {
          vertex_identity_describe(&attachment->vertex, builder_writer);
        } else {
          STRING("_")
        }
      }
      { MAPPING STRING("target")
        if(target) {
          vertex_identity_describe(&target->vertex, builder_writer);
        } else {
          STRING("_")
        }
      }
      { MAPPING STRING("binding")
        if(binding) {
          vertex_identity_describe(&binding->vertex, builder_writer);
        } else {
          STRING("_")
        }
      }
    }
  }

printf("%.*s\n", (int)growing.l, growing.s);
  PROBE1(bind, attach, growing.s);
  bacon_writer_destroy(&writer);
  narrator_growing_destroy(&growing);
}
