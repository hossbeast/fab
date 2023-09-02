/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "types.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "common/attrs.h"
#include "fab/ipc.h"
#include "fab/events.h"

#include "channel.h"
#include "probes.h"
#include "zcat.h"

#include "usdt-sem.h"

#if 1
PROBE_SEM(channel, rx) = 0;
PROBE_SEM(channel, tx) = 0;
#else
PROBE_SEM(channel, rx) = 1;
PROBE_SEM(channel, tx) = 1;
#endif

void channel_rx_probe(const channel * restrict chan, fabipc_message * restrict msg)
{

}

void channel_tx_probe(const channel * restrict chan, fabipc_message * restrict msg)
{
  uint32_t h;
  uint32_t i;
  char buf[64];
  size_t z;
  fabipc_page *page;
  narrator *N;
  narrator_fixed fixed;
  char text[512];

  if(!channel_tx_semaphore) {
    return;
  }

  N = narrator_fixed_init(&fixed, text, sizeof(text));
  page = containerof(msg, fabipc_page, msg);
  h = page->tail;
  i = page - chan->ipc.server_ring.pages;
  z = attrs32_zcat(buf, sizeof(buf), fabipc_msg_type_attrs, msg->type);
  narrator_sayf(N, "tx tail %5u idx %5u id 0x%016"PRIx64" code %8d type %.*s", h, i, msg->id, msg->code, (int)z, buf);
  if(msg->type == FABIPC_MSG_EVENTS)
  {
    z = attrs32_zcat(buf, sizeof(buf), fabipc_event_type_attrs, msg->evtype);
    narrator_sayf(N, " attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    narrator_sayf(N, " attrs 0x%08x", msg->attrs);
  }
  narrator_sayf(N, " size %hu", msg->size);
  if(msg->type == FABIPC_MSG_RESPONSE || (msg->type == FABIPC_MSG_EVENTS && msg->evtype == FABIPC_EVENT_SYSTEM_STATE))
  {
    narrator_sayf(N, " %.*s", (int)msg->size, msg->text);
  }

printf("%.*s\n", (int)fixed.l, fixed.s);
  PROBE1(channel, rx, fixed.s);
}
