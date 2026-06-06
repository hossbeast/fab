/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "command.h"
#include "bam/ipc.h"
#include "bam/client.h"
#include "bam/events.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "common/parseint.h"

#include "params.h"
#include "info.h"
#include "params.h"
#include "zcat.h"

typeof(g_cmd) g_cmd = (typeof(*g_cmd)[]) {{ }};

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

void command_context(bacon_writer * restrict builder_writer)
{
  if(g_args.context_module_use) {
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/modules/%.*s", g_args.context_module_id_len, g_args.context_module_id)
            }
            { MAPPING STRING("nofollow")
              BOOL(false)
            }
            { MAPPING STRING("expect")
              if(g_args.context_module_expect) {
                STRING("one")
              } else {
                STRING("any")
              }
            }
          }
        }
        STRING("module-of")
      }
    }
    STRING("context-module")
  }
}

uint64_t command_post(struct command * restrict cmd, bam_client * restrict client, command_builder *builder)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  msg = client_tx(client);
  msg->type = BAMIPC_MSG_REQUEST;
  msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  builder(cmd, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->len = fixed.l;
  client_publish(client, msg);

  return msg->id;
}

size_t command_identify_result_parse(char *dst, size_t dst_size, bamipc_message * restrict msg)
{
  const char *s;
  const char *e;

  if(msg->len < 2) {
    return 0;
  }

  /* trim leading and trailing whitespace and punctuation */
  s = msg->text;
  while((s - msg->text) != msg->len) {
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

  e = msg->text + msg->len - 1;
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

//
//
//

bamipc_message *client_tx(bam_client * restrict client)
{
  bamipc_message *msg;
  int r;

  if((r = bam_client_tx(client, 0, &msg)) == BAMIPC_OK) {
    msg->len = 0;
    return msg;
  } else {
    RTA;
  }

  return msg;
}

int client_rx(bam_client * restrict client, struct timespec * restrict timeout, bamipc_message ** restrict msgp)
{
  bamipc_message *msg;
  char type[64];
  char code[64];
  char format[64];
  size_t z;
  int r;

  r = bam_client_rx(client, timeout, msgp);
  if(r == BAMIPC_EINTR || r == BAMIPC_ETIMEDOUT) {
    return r;
  }
  if(g_args.verbose <= 2) {
    return r;
  }

  msg = *msgp;
  if(msg->type == BAMIPC_MSG_PONG && g_args.verbose <= 5) {
    return r;
  }

  if(attrs32_zcat(code, sizeof(code), &bamipc_msg_code_attrs, msg->code) == 0) {
    zcatf(code, sizeof(code), "%u", msg->code);
  }
  if(attrs32_zcat(format, sizeof(format), &bamipc_msg_format_attrs, msg->format) == 0) {
    zcatf(format, sizeof(format), "%u", msg->format);
  }

  attrs32_zcat(type, sizeof(type), &bamipc_msg_type_attrs, msg->type);
  printf("rx slot %5u idx %5u id 0x%016"PRIx64" type %-8s format %-8s code %-8s", msg->slot, msg->index, msg->id, type, format, code);
  if(msg->type == BAMIPC_MSG_EVENTS)
  {
    z = attrs32_zcat(type, sizeof(type), &bamipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, type);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu ", msg->len);
  if(msg->type == BAMIPC_MSG_EVENTS)
  {
    switch(msg->evtype) {
      case BAMIPC_EVENT_FORMULA_FORK_BIN:
      case BAMIPC_EVENT_FORMULA_REAP_BIN:
      case BAMIPC_EVENT_FORMULA_AUXOUT_BIN:
      case BAMIPC_EVENT_FORMULA_STDOUT_BIN:
      case BAMIPC_EVENT_FORMULA_STDERR_BIN:
        break;

      case BAMIPC_EVENT_SYSTEM_STATE_CHANGE:
        if(msg->len) {
          printf("%.*s", (int)msg->len, msg->text);
        }
        break;
      default:
        if(g_args.verbose >= 3) {
          printf("\n%.*s", (int)msg->len, msg->text);
        }
        break;
    }
    printf("\n");
  } else {
    printf("\n");
    if(g_args.verbose >= 3 && msg->len) {
      printf("%.*s\n", (int)msg->len, msg->text);
    }
  }

  return r;
}

static void client_tx_show(const bamipc_message * restrict msg)
{
  char buf[64];
  size_t z;

  if(g_args.verbose < 1) {
    return;
  }
  if(msg->type == BAMIPC_MSG_PING && g_args.verbose < 4) {
    return;
  }

  z = attrs32_zcat(buf, sizeof(buf), &bamipc_msg_type_attrs, msg->type);
  printf("tx slot %5u idx %5u id 0x%016"PRIx64" code %8d type %.*s", msg->slot, msg->index, msg->id, msg->code, (int)z, buf);
  if(msg->type == BAMIPC_MSG_EVENTS)
  {
    z = attrs32_zcat(buf, sizeof(buf), &bamipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" len %hu\n", msg->len);
  if(msg->len) {
    printf("%.*s\n", (int)msg->len, msg->text);
  }
}

void client_publish(struct bam_client * restrict client, bamipc_message * restrict msg)
{
  client_tx_show(msg);
  bam_client_publish(client, msg);
}

void client_ping(struct bam_client * restrict client, uint64_t msgid)
{
  bamipc_message * msg;

  msg = client_tx(client);
  msg->id = msgid;
  msg->type = BAMIPC_MSG_PING;
  client_publish(client, msg);
}

void client_pong(struct bam_client * restrict client, uint64_t msgid)
{
  bamipc_message * msg;

  msg = client_tx(client);
  msg->id = msgid;
  msg->type = BAMIPC_MSG_PONG;
  client_publish(client, msg);
}
