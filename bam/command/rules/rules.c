/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bam/describe.h"
#include "bam/describe.desc.h"
#include "bam/rule_info.desc.h"
#include "bam/rule_info.h"
#include "bam/stats.h"
#include "bam/stats.desc.h"
#include "bam/rules_describe.h"
#include "bam/rules_describe.desc.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "rules.h"
#include "params.h"
#include "zcat.h"

static uint64_t requestid;
struct rules_args rules_args;
struct rule_args rule_args;

/* state */
static bam_rules_describe_result describe_result, *result;
static bam_rule_stats rule_stats[64];
static uint16_t rule_stats_len;
static bam_rule_info rule_info[64];
static uint16_t rule_info_len;
static bool any;

//
// rule
//

static void rule_usage(command * restrict cmd)
{
  printf(
"rule options (none)\n"
  );
}

static void rule_connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  /* required for this subcommand */
  RUNTIME_ASSERT(g_args.context_module_id);

  { LIST
    { MAPPING
      STRING("context")
      { SET
        { MAPPING
          STRING("module")
          HEX(g_args.context_module_id)
        }
      }
    }
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("lookup")
          { SET
            { MAPPING
              STRING("path")
              LOADF("//module/rma/%.*s", rule_args.target.len, rule_args.target.s)
            }
          }
        }
      }
    }
    STRING("rma-describe")
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->size = fixed.l;
  client_post(client, msg);
}

static void rule_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id != requestid) {
    return;
  }

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    return;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);
  write(1, msg->text, msg->size);
  any = true;
}

static void rule_attached(command * restrict cmd, bam_client * restrict client)
{

}

static void rule_disconnected(command * restrict cmd)
{
  if(any) {
    printf("\n");
  }
}

//
// rules
//

static void rules_usage(command * restrict cmd)
{
  printf(
"\n"
"rules options\n"
  );
}

static void rules_connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  /* required for this subcommand */
  RUNTIME_ASSERT(g_args.context_module_id);

  { LIST
    { MAPPING
      STRING("context")
      { SET
        { MAPPING
          STRING("module")
          HEX(g_args.context_module_id)
        }
      }
    }
    STRING("rules-describe")
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->size = fixed.l;
  client_post(client, msg);
}

static void rules_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  bam_rule_stats *stats;
  bam_rule_info *info;
  bam_rules_describe_string string;
  size_t z;
  int x;

  if(msg->id != requestid) {
    return;
  }

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    goto end;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);

  switch(msg->attrs) {
    case DESCRIPTOR_ID_bam_rules_describe_result:
      descriptor_type_unmarshal(&describe_result, &descriptor_bam_rules_describe_result, msg->text, msg->size);
      result = &describe_result;
      break;
    case DESCRIPTOR_ID_bam_rule_stats:
      descriptor_type_unmarshal(&rule_stats[rule_stats_len++], &descriptor_bam_rule_stats, msg->text, msg->size);
      break;
    case DESCRIPTOR_ID_bam_rule_info:
      descriptor_type_unmarshal(&rule_info[rule_info_len++], &descriptor_bam_rule_info, msg->text, msg->size);
      break;
    default:
      RTA;
  }
  return;

end:

  if(!result) {
    return;
  }

  printf("# %-11s %.*s\n", "directory", result->dir_abspath_len, result->dir_abspath);
  printf("# %-11s %.*s id %.*s\n", "module", result->mod_id_len, result->mod_id, result->mod_abspath_len, result->mod_abspath);
  printf("#\n");
  printf("# rules files %u\n", result->file_list_len);
  printf("# rules %u\n", rule_stats_len);
  printf("#\n");

  z = 0;
  for(x = 0; x < result->file_list_len; x++) {
    z += descriptor_type_unmarshal(&string, &descriptor_bam_rules_describe_string, result->file_list + z, result->file_list_size - z);
    printf("# %.*s\n", string.text_len, string.text);
  }
  RUNTIME_ASSERT((z + 6) == result->file_list_size);
  printf("#\n");

  if(rule_stats_len) {
    printf("\n");
  }

  for(x = 0; x < rule_stats_len; x++) {
    stats = &rule_stats[x];
    info = &rule_info[x];

    printf("# %.*s @ [%3hu,%3hu - %3hu,%3hu)\n", (int)info->file_abspath_len, info->file_abspath, info->f_lin, info->f_col, info->l_lin, info->l_col);
    printf("#  %-8s : %u\n", "rule_run", stats->rule_run);
    printf("%.*s\n", (int)info->rule_text_len, info->rule_text);
    printf("\n");
  }
}

static void rules_attached(command * restrict cmd, bam_client * restrict client)
{
}

static void rules_disconnected(command * restrict cmd)
{

}

struct command rule_command = {
    name : "rule"
  , attrs : COMMAND_CONTEXT_MODULE
  , attached : rule_attached
  , usage : rule_usage
  , connected : rule_connected
  , process : rule_process
  , disconnected : rule_disconnected
};

struct command rules_command = {
    name : "rules"
  , attrs : COMMAND_CONTEXT_MODULE
  , attached : rules_attached
  , usage : rules_usage
  , connected : rules_connected
  , process : rules_process
  , disconnected : rules_disconnected
};
