/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "common/attrs.h"
#include "bam/client.h"
#include "bam/vertex.h"
#include "bam/ipc.h"
#include "bam/stats.desc.h"
#include "bam/stats.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "stats.h"
#include "params.h"
#include "zcat.h"
#include "marshal.h"

struct stats_args stats_args;
static struct stats_args *args = &stats_args;
static uint64_t requestid;

static void usage(command * restrict cmd)
{
  printf(
"\n"
"stats options\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  int x;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  /* send the request */
  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  if(args->targets_len == 0)
  {
    if(args->reset) {
      STRING("global-stats-reset")
    } else {
      STRING("global-stats-read")
    }
  }
  else
  {
    LIST
    if(g_args.context_module_id) {
      MAPPING
      STRING("context")
      { SET
        { MAPPING
          STRING("module")
          HEX(g_args.context_module_id)
        }
      }
    }
    { MAPPING
      { STRING("select")
        { LIST
          for(x = 0; x < args->targets_len; x++)
          {
            { MAPPING
              STRING("lookup")
              { SET
                { MAPPING
                  STRING("path")
                  BYTES(args->targets[x].s, args->targets[x].len)
                }
                { MAPPING
                  STRING("hidden")
                  BOOL(true)
                }
              }
            }
          }
        }
      }
    }
    { MAPPING
      if(args->reset) {
        STRING("stats-reset")
      } else {
        STRING("stats-read")
      }
      { SET
        if(args->special_only) {
          STRING("special-only")
        }
        if(args->no_special) {
          STRING("no-special")
        }
      }
    }
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->size = fixed.l;
  client_post(client, msg);
}

static int show_handler_fn(
    const descriptor_type * restrict type
  , const descriptor_field * restrict member
  , const void * restrict object
  , struct descriptor_unmarshal_handler *handler
)
{
  const bam_stats_record *record;
  const attrs16 *attrs = 0;
  uint64_t u = 0;

  record = object;
  if(member == &descriptor_bam_global_stats_system_state) {
    attrs = bam_system_state_attrs;
  } else if(member == &descriptor_bam_vertex_stats_type) {
    attrs = bam_vertex_type_attrs;
  } else if(member == &descriptor_bam_vertex_stats_state) {
    attrs = bam_vertex_state_attrs;
  } else {
    return 0;
  }

  switch(member->size) {
    case 8:
      u = record->u64[member->offset / 8];
      break;
    case 4:
      u = record->u32[member->offset / 4];
      break;
    case 2:
      u = record->u16[member->offset / 2];
      break;
    case 1:
      u = record->u8[member->offset / 1];
      break;
  }

  printf("%35.*s : %s\n", (int)member->name_len, member->name, attrs16_name_byvalue(attrs, u));
  return 1;
}

static struct {
  descriptor_unmarshal_handler unmarshal;
} show_handler = {
  .unmarshal.fn = show_handler_fn
};

static void show(descriptor_type * restrict descriptor, bam_stats_record * restrict record, descriptor_unmarshal_handler *handler)
{
  descriptor_field *member;
  size_t z;
  int x;

  for(x = 0; x < descriptor->members_len; x++)
  {
    member = descriptor->members[x];

    z = 0;
    if(handler) {
      z = handler->fn(descriptor, member, record, handler);
    }
    if(z != 0) {
      continue;
    }

    if(member->code == DESCRIPTOR_STRING) {
      printf("%35.*s : %.*s\n", (int)member->name_len, member->name, record->u16[member->len_offset / 2], record->s[member->offset / sizeof(void*)]);
      continue;
    }

#define CASE2(x, y) ((x << 16) | y)
    switch(CASE2(member->code, member->size)) {
      case CASE2(DESCRIPTOR_UINT, 8):
        printf("%35.*s : %"PRIu64"\n", (int)member->name_len, member->name, record->u64[member->offset / 8]);
        break;
      case CASE2(DESCRIPTOR_UINT, 4):
        printf("%35.*s : %"PRIu32"\n", (int)member->name_len, member->name, record->u32[member->offset / 4]);
        break;
      case CASE2(DESCRIPTOR_UINT, 2):
        printf("%35.*s : %"PRIu16"\n", (int)member->name_len, member->name, record->u16[member->offset / 2]);
        break;
      case CASE2(DESCRIPTOR_UINT, 1):
        printf("%35.*s : %"PRIu8"\n", (int)member->name_len, member->name, record->u8[member->offset / 1]);
        break;
      default:
        RTA;
    }
  }
}

static void process_msg(bamipc_message * restrict msg)
{
  union {
    char pad[512];
    bam_stats_record record;
  } stats;
  descriptor_type *descriptor;
  size_t z;
  void *src = msg->text;
  size_t sz = msg->size;
  uint32_t id;

  z = 0;

  descriptor = &descriptor_bam_vertex_stats;
  while(z < msg->size)
  {
    /* peek at the id */
    unmarshal_u32(src + z, sz - z, &id);

    switch(id) {
      /* global stats */
      case DESCRIPTOR_ID_bam_global_stats:
        descriptor = &descriptor_bam_global_stats;
        break;
      /* node stats */
      case DESCRIPTOR_ID_bam_formula_stats:
        descriptor = &descriptor_bam_formula_stats;
        break;
      case DESCRIPTOR_ID_bam_rule_stats:
        descriptor = &descriptor_bam_rule_stats;
        break;
      case DESCRIPTOR_ID_bam_module_stats:
        descriptor = &descriptor_bam_module_stats;
        break;
      case DESCRIPTOR_ID_bam_rules_stats:
        descriptor = &descriptor_bam_rules_stats;
        break;
      case DESCRIPTOR_ID_bam_vertex_stats:
        descriptor = &descriptor_bam_vertex_stats;
        break;
      case DESCRIPTOR_ID_bam_config_stats:
        descriptor = &descriptor_bam_config_stats;
        break;
      case DESCRIPTOR_ID_bam_var_stats:
        descriptor = &descriptor_bam_var_stats;
        break;
      case DESCRIPTOR_ID_bam_fsent_dir_stats:
        descriptor = &descriptor_bam_fsent_dir_stats;
        break;
      default:
        RTA;
    }

    z += descriptor_type_unmarshal(&stats, descriptor, src + z, sz - z);
    show(descriptor, &stats.record, &show_handler.unmarshal);
  }

  RUNTIME_ASSERT(z == msg->size);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  RUNTIME_ASSERT(msg->id == requestid);

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    return;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);
  process_msg(msg);
}

static void attached(command * restrict cmd, bam_client * restrict client)
{

}

//
// public
//

static void disconnected(command * restrict cmd) { }
struct command stats_command = {
    usage : usage
  , attached : attached
  , connected : connected
  , process : process
  , disconnected : disconnected
};
