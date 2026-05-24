/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "types.h"
#include "narrator/fixed.h"
#include "bam/events.h"

#include "channel.h"
#include "probes.h"
#include "zcat.h"

#include "usdt-sem.h"

PROBE_SEM(channel, rx) = 0;
PROBE_SEM(channel, tx) = 0;

static void message_say(bamipc_message * restrict msg, narrator * restrict N)
{
  uint32_t i;
  size_t z;
  char buf[64];
  char code[64];

  i = msg->index;

  if(attrs32_zcat(code, sizeof(code), &bamipc_msg_code_attrs, msg->code) == 0) {
    zcatf(code, sizeof(code), "%u", msg->code);
  }

  z = attrs32_zcat(buf, sizeof(buf), &bamipc_msg_type_attrs, msg->type);
  narrator_sayf(N, "index %5u id 0x%016"PRIx64" code %-8s type %.*s", i, msg->id, code, (int)z, buf);
  if(msg->type == BAMIPC_MSG_EVENTS)
  {
    z = attrs32_zcat(buf, sizeof(buf), &bamipc_event_type_attrs, msg->evtype);
    narrator_sayf(N, " evtype %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    narrator_sayf(N, " attrs 0x%08x", msg->attrs);
  }
  narrator_sayf(N, " size %hu", msg->len);

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
          if(0) {
            printf("%.*s", (int)msg->len, msg->text);
          }
        }
        break;
      case BAMIPC_EVENT_BUILD_START:
      case BAMIPC_EVENT_BUILD_END:
      case BAMIPC_EVENT_GOALS_SET:
        if(msg->len) {
          if(1) {
            printf("\n%.*s", (int)msg->len, msg->text);
          }
        }
        break;
    }
    printf("\n");
  } else {
    printf("\n");
    if(msg->len) {
      if(1) {
        printf("len %u addr %p\n", msg->len, msg->text);
        //printf("%.*s\n", (int)msg->len, msg->text);
      }
    }
  }
}

void channel_rx_probe(const channel * restrict chan, bamipc_message * restrict msg)
{
  narrator *N;
  narrator_fixed fixed;
  char text[PATH_MAX];

  if(!channel_rx_semaphore) {
    return;
  }

  N = narrator_fixed_init(&fixed, text, sizeof(text));
  narrator_says(N, "rx ");
  message_say(msg, N);

printf("%.*s\n", (int)fixed.l, fixed.s);
  PROBE1(channel, rx, fixed.s);
}

void channel_tx_probe(const channel * restrict chan, bamipc_message * restrict msg)
{
  narrator *N;
  narrator_fixed fixed;
  char text[PATH_MAX];

  if(!channel_tx_semaphore) {
    return;
  }

  N = narrator_fixed_init(&fixed, text, sizeof(text));
  narrator_says(N, "tx ");
  message_say(msg, N);

printf("%.*s\n", (int)fixed.l, fixed.s);
  PROBE1(channel, tx, fixed.s);
}
