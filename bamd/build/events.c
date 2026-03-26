/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "bam/ipc.h"
#include "bam/events.h"
#include "xlinux/xtime.h"

#include "build.h"
#include "build/events.h"
#include "build_record.h"
#include "events/events.h"
#include "buildplan.h"
#include "build_slot.h"
#include "bpe.h"
#include "formula.h"
#include "times.h"

void build_event_start(build_record *rec)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  bamipc_message *msg;
  events_context ectx;

  build_stats.build++;

  memset(&build_stats._last, 0, sizeof(build_stats) - offsetof(typeof(build_stats), _last));
  build_stats.last_slots = llist_count(&buildplan_selection.list);
  build_stats.last_stages = buildplan_selection.numranks;

  xclock_gettime(CLOCK_MONOTONIC_COARSE, &rec->start_time);

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_BUILD_START))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("build-start")
    { SET
      { MAPPING STRING("id") BYTES(rec->id, rec->id_len) }
      { MAPPING STRING("token") HEX64(rec->token) }
      { MAPPING STRING("label") BYTES(rec->label, rec->label_len) }
      { MAPPING STRING("pid") UINT(rec->pid) }
      { MAPPING STRING("autorun") BOOL(rec->autorun) }
      { MAPPING STRING("slots") UINT(buildplan_selection.selected_entities.size) }
      { MAPPING STRING("stages") UINT(buildplan_selection.numranks) }
    }
  }

  msg->id = 0;
  msg->len = fixed.l;
  events_publish(&ectx);
}

void build_event_end(build_record * restrict rec)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  bamipc_message *msg;
  events_context ectx;
  char buf[256];

  xclock_gettime(CLOCK_MONOTONIC_COARSE, &rec->end_time);

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_BUILD_END))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("build-end")
    { SET
      { MAPPING STRING("id") BYTES(rec->id, rec->id_len) }
      { MAPPING STRING("token") HEX64(rec->token) }
      { MAPPING STRING("label") BYTES(rec->label, rec->label_len) }
      { MAPPING STRING("pid") UINT(rec->pid) }
      { MAPPING STRING("autorun") BOOL(rec->autorun) }
      { MAPPING STRING("runtime")
        timespec_ns_cat(buf, sizeof(buf), timespec_delta(&rec->start_time, &rec->end_time));
        STRING(buf)
      }
      { MAPPING STRING("success") BOOL(rec->slot_fail == 0 && rec->slot_prep_fail == 0) }
      { MAPPING STRING("slot-pass") UINT(rec->slot_pass) }
      { MAPPING STRING("slot-fail") UINT(rec->slot_fail) }
      { MAPPING STRING("slot-prep-fail") UINT(rec->slot_prep_fail) }
    }
  }

  msg->id = 0;
  msg->len = fixed.l;
  events_publish(&ectx);
}

void build_event_ok()
{
    build_stats.build_ok++;
    build_stats.last_build_ok++;
}

void build_event_error()
{
    build_stats.build_err++;
    build_stats.last_build_err++;
}

void build_event_slot_ok(build_slot * restrict bs)
{
  build_stats.slot_ok++;
  build_stats.last_slot_ok++;

  if(bs) {
    build_stats.fml_exec_ok++;
    build_stats.last_fml_exec_ok++;
    bs->bpe->bpe_stats.build_ok++;
    bs->bpe->fml->formula_stats.fml_exec_ok++;
  }
}

void build_event_slot_err(build_slot * restrict bs)
{
  build_stats.slot_err++;
  build_stats.last_slot_err++;

  bs->bpe->bpe_stats.build_err++;
  bs->bpe->fml->formula_stats.fml_exec_err++;
}
