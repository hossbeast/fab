/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "command.h"
#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "common/parseint.h"

#include "params.h"
#include "info.h"
#include "params.h"

typeof(g_cmd) g_cmd = (typeof(*g_cmd)[]) {{ }};

uint64_t message_no_error_list[8];
uint16_t message_no_error_list_len;

struct attrs32 command_attrs = (attrs32) {
#undef DEF
#define DEF(x, y) + 1
    num : 0
      COMMAND_ATTRS_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, y) { name : #x, value : y, range : COMMAND_OPT },
COMMAND_ATTRS_TABLE
#undef DEF
  }
};

void command_context(command * restrict cmd, bacon_writer * restrict builder_writer)
{
  if(g_args.no_context_module) {
    return;
  }

  { MAPPING
    STRING("context")
    { SET
      { MAPPING
        STRING("module")
        BYTES(g_args.context_module_id, g_args.context_module_id_len)
      }
    }
  }
}

uint64_t command_post(struct command * restrict cmd, bam_client * restrict client, command_builder *builder)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  builder(cmd, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->size = fixed.l;
  client_post(client, msg);

  return msg->id;
}

size_t command_identify_result_parse(bamipc_message * restrict msg, char *dst, size_t dst_size)
{
  const char *s;
  const char *e;

  if(msg->size < 2) {
    return 0;
  }

  /* trim leading and trailing whitespace and punctuation */
  s = msg->text;
  while((s - msg->text) != msg->size) {
    if(*s == ' ' || *s == '\n') {
      s++;
    } else if(*s == '{') {
      s++;
    } else if(*s == '"') {
      s++;
    } else {
      break;
    }
  }

  e = msg->text + msg->size - 1;
  while(e != msg->text) {
    if(*e == ' ' || *e == '\n') {
      e--;
    } else if(*e == '}') {
      e--;
    } else if(*e == '"') {
      e--;
    } else {
      break;
    }
  }
  e++;

  if(e <= s) {
    return 0;
  }

  memcpy(dst, s, e - s);

  return e - s;
}

void command_set(struct command * restrict cmd)
{
  memcpy(g_cmd, cmd, sizeof(*g_cmd));
}

void command_sub_set(struct command * restrict cmd)
{
  memcpy(g_cmd, cmd, offsetof(typeof(*cmd), _sub_command_state));
}
