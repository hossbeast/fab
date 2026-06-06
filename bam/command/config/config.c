/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/ipc.h"
#include "bam/client.h"
#include "bam/config_tell.h"
#include "bam/config_tell.desc.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "types.h"
#include "config.h"
#include "params.h"

static uint64_t requestid;
static bam_config_tell_result result;
static bamipc_message *texts[64];
static uint16_t texts_len;

static void usage(command * restrict cmd)
{
  printf(
"config options (none)\n"
  );
}

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  msg = client_tx(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  { LIST
    { MAPPING
      STRING("select")
      { LIST
        MAPPING
        STRING("path")
        { SET
          { MAPPING STRING("pattern")
            LOADF("//bm/sys-config")
          }
        }
      }
    }

    STRING("tell")
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->len = fixed.l;
  client_publish(client, msg);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  int x;
  bam_config_tell_item item;
  size_t z;

  if(msg->id != requestid) {
    return;
  }

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    goto end;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);

  switch(msg->format) {
    case DESCRIPTOR_ID_bam_config_tell_result:
      descriptor_type_unmarshal(&result, &descriptor_bam_config_tell_result, msg->text, msg->len);
      break;
    case BAMIPC_MSG_BACON:
    case BAMIPC_MSG_TEXT:
      texts[texts_len++] = msg;
      break;
    default:
      RTA;
  }
  return;

end:
  printf("#\n");
  printf("# config files %u\n", result.file_list_len);

  z = 0;
  for(x = 0; x < result.file_list_len; x++) {
    z += descriptor_type_unmarshal(&item, &descriptor_bam_config_tell_item, result.file_list + z, result.file_list_size - z);
    printf("#  %.*s %.*s\n", item.id_len, item.id, item.abspath_len, item.abspath);
  }
  RUNTIME_ASSERT((z + 6) == result.file_list_size);
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

typeof(config_command) config_command = {
    name : "config"
  , usage : usage
  , connected : connected
  , process : process
};
