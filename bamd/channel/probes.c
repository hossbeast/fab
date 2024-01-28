/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "types.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "common/attrs.h"
#include "bam/ipc.h"
#include "bam/events.h"

#include "channel.h"
#include "probes.h"
#include "zcat.h"

#include "usdt-sem.h"

PROBE_SEM(channel, rx) = 0;
PROBE_SEM(channel, tx) = 0;

void channel_rx_probe(const channel * restrict chan, bamipc_message * restrict msg)
{

}

void channel_tx_probe(const channel * restrict chan, bamipc_message * restrict msg)
{
  uint32_t h;
  uint32_t i;
  char buf[64];
  size_t z;
  bamipc_page *page;
  narrator *N;
  narrator_fixed fixed;
  char text[PATH_MAX];

  if(!channel_tx_semaphore) {
    return;
  }

  N = narrator_fixed_init(&fixed, text, sizeof(text));
  page = containerof(msg, bamipc_page, msg);
  h = page->tail;
  i = page - chan->ipc.server_ring.pages;
  z = attrs32_zcat(buf, sizeof(buf), bamipc_msg_type_attrs, msg->type);
  narrator_sayf(N, "tx tail %5u idx %5u id 0x%016"PRIx64" code %8d type %.*s", h, i, msg->id, msg->code, (int)z, buf);
  if(msg->type == BAMIPC_MSG_EVENTS)
  {
    z = attrs32_zcat(buf, sizeof(buf), bamipc_event_type_attrs, msg->evtype);
    narrator_sayf(N, " attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    narrator_sayf(N, " attrs 0x%08x", msg->attrs);
  }
  narrator_sayf(N, " size %hu", msg->size);
  if(msg->type == BAMIPC_MSG_RESPONSE || (msg->type == BAMIPC_MSG_EVENTS && msg->evtype == BAMIPC_EVENT_SYSTEM_STATE))
  {
    narrator_sayf(N, " %.*s", (int)msg->size, msg->text);
  }

printf("%.*s\n", (int)fixed.l, fixed.s);
  PROBE1(channel, rx, fixed.s);
}
