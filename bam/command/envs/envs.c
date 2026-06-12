/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"
#include "bacon/builder.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bam/envs_tell.h"
#include "bam/envs_tell.desc.h"

#include "envs.h"
#include "params.h"
#include "args.h"

static bam_envs_tell_result result;
static bamipc_message *texts[512];
static uint16_t texts_len;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"module envs options (none)\n"
  );
}

static void envs_command_tell(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING
              STRING("pattern")
              LOADF("//bm/modules/%.*s/envs", cmd->module_id_len, cmd->module_id)
            }
          }
        }
      }
    }

    STRING("tell")
  }
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  bam_envs_tell_item item;
  size_t z;
  int x;

  if(msg->id == cmd->identify_id_0 || msg->id == cmd->identify_id_1) {
    /* identify : resolve the module target -> module id */
    if(module_identify_msg(cmd, client, msg)) {
      cmd->tell_id = command_post(cmd, client, envs_command_tell);
    }
    return;
  }

  if(msg->id == cmd->tell_id && msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    goto show;
  } else if(msg->id == cmd->tell_id && msg->type == BAMIPC_MSG_RESULT) {
    switch(msg->format) {
      case DESCRIPTOR_ID_bam_envs_tell_result:
        descriptor_type_unmarshal(&result, &descriptor_bam_envs_tell_result, msg->text, msg->len);
        break;
      case BAMIPC_MSG_BACON:
      case BAMIPC_MSG_TEXT:
        RUNTIME_ASSERT(texts_len < ARRAY_LEN(texts));
        texts[texts_len++] = msg;
        break;
      default:
        RTA;
    }
  } else {
    RTA;
  }
  return;

show:
  printf("#\n");
  printf("# %s\n", "module");
  printf("#  %.*s %.*s\n", result.module_id_len, result.module_id, result.module_abspath_len, result.module_abspath);
  printf("#\n");
  printf("# var files %u\n", result.file_list_len);

  z = 0;
  for(x = 0; x < result.file_list_len; x++) {
    z += descriptor_type_unmarshal(&item, &descriptor_bam_envs_tell_item, result.file_list + z, result.file_list_size - z);
    printf("#  %.*s %.*s\n", item.id_len, item.id, item.abspath_len, item.abspath);
  }
  printf("#\n");

  if(texts_len) {
    printf("\n");
  }

  for(x = 0; x < texts_len; x++) {
    (void)!write(1, texts[x]->text, texts[x]->len);
  }

  (void)!write(1, "\n", 1);
}

//
// public
//

typeof(envs_command) envs_command = {
    name : "envs"
  , usage : usage
  , connected : module_command_connected
  , process : process
};
