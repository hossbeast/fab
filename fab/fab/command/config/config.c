/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "fab/ipc.h"
#include "fab/client.h"
#include "fab/config_describe.h"
#include "fab/config_describe.desc.h"

#include "config.h"
#include "params.h"
#include "zcat.h"

static uint64_t requestid;
static bam_config_describe_result result;
static fabipc_message *texts[64];
static uint16_t texts_len;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"config options (none)\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, fab_client * restrict client)
{
  fabipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  { LIST
    STRING("config-describe")
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->size = fixed.l;
  client_post(client, msg);
}

static void process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  int x;
  bam_config_describe_string string;
  size_t z;

  if(msg->id != requestid) {
    return;
  }

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    goto end;
  }

  RUNTIME_ASSERT(msg->type == FABIPC_MSG_RESULT);

  switch(msg->attrs) {
    case DESCRIPTOR_ID_bam_config_describe_result:
      descriptor_type_unmarshal(&result, &descriptor_bam_config_describe_result, msg->text, msg->size);
      break;
    case 0:
      texts[texts_len++] = msg;
      break;
    default:
      RTA;
  }
  return;

end:
  printf("# config files %u\n", result.file_list_len);
  printf("#\n");

  z = 0;
  for(x = 0; x < result.file_list_len; x++) {
    z += descriptor_type_unmarshal(&string, &descriptor_bam_config_describe_string, result.file_list + z, result.file_list_size - z);
    printf("# %.*s\n", string.text_len, string.text);
  }
  RUNTIME_ASSERT((z + 6) == result.file_list_size);
  printf("#\n");

  if(texts_len) {
    printf("\n");
  }

  for(x = 0; x < texts_len; x++) {
    write(1, texts[x]->text, texts[x]->size);
  }

  write(1, "\n", 1);
}

static void attached(command * restrict cmd, fab_client * restrict client)
{

}

//
// public
//

static void disconnected(command * restrict cmd) { }

struct command config_command = {
    name : "config"
  , attached : attached
  , usage : usage
  , connected : connected
  , process : process
  , disconnected : disconnected
};
