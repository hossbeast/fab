/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "xlinux/xshm.h"
#include "bam/ipc.h"

#include "channel.h"
#include "configset.h"
#include "params.h"

/* ugo+rw  world read/write */
#define SHMCHANNEL_MODE 0666

#include <inttypes.h>
#include "bam/events.h"
#include "common/attrs.h"
#include "channel/probes.h"
#include "config.h"

//
// public
//

void channel_create(channel ** restrict chanp, pid_t thread_id)
{
  channel *chan;
  int shmid = -1;
  void * shmaddr = 0;

  shmid = xshmget(IPC_PRIVATE, BAMIPC_SHMSIZE, IPC_CREAT | IPC_EXCL | SHMCHANNEL_MODE);
  shmaddr = xshmat(shmid, 0, 0);
  xshmctl(shmid, IPC_RMID, 0);

  chan = shmaddr;
  chan->ipc.shmid = shmid;
  chan->ipc.server_pid = g_params.pid;
  chan->ipc.server_tid = thread_id;
  chan->ipc.server_pulse = thread_id;
  *chanp = chan;
}

void channel_release(channel * restrict chan)
{
  shmdt(chan);
}

/* TX : produce -> post */

bamipc_message * channel_produce(channel * restrict chan)
{
  bamipc_message *msg;

  msg = bamipc_produce(
      chan->ipc.server_ring.pages
    , &chan->ipc.server_ring.head
    , &chan->ipc.server_ring.tail
    , &chan->ipc.server_ring.producers
    , &chan->ipc.server_ring.waiters
  );
  RUNTIME_ASSERT(msg);

  msg->id = chan->msgid;

  return msg;
}

void channel_post(channel * restrict chan, bamipc_message * restrict msg)
{
#if CONFIG_DEBUG || CONFIG_DEVEL
  RUNTIME_ASSERT(msg->type);
  if(msg->type == BAMIPC_MSG_EVENTS) {
    RUNTIME_ASSERT(msg->evtype);
  }
#endif

  if(msg->type == BAMIPC_MSG_RESPONSE && msg->code != 0) {
    chan->error = true;
  }

  bamipc_post(
      msg
    , &chan->ipc.server_ring.producers
    , &chan->ipc.server_ring.waiters
  );

  channel_tx_probe(chan, msg);
}

/* RX : acquire -> consume */

bamipc_message * channel_acquire(channel * restrict chan)
{
  bamipc_message *msg;

  msg = bamipc_acquire(
      chan->ipc.client_ring.pages
    , &chan->ipc.client_ring.head
    , &chan->ipc.client_ring.tail
  );

#if CONFIG_DEBUG || CONFIG_DEVEL
  if (msg) {
    RUNTIME_ASSERT(msg->type);
  }
#endif

  if(msg) {
    channel_rx_probe(chan, msg);
  }

  return msg;
}

void channel_consume(channel * restrict chan, bamipc_message * restrict msg)
{
  bamipc_consume(
      chan->ipc.client_ring.pages
    , &chan->ipc.client_ring.head
    , msg
    , &chan->ipc.client_ring.waiters
  );
}

void channel_response(channel * restrict chan, int code)
{
  channel_responsew(chan, code, 0, 0);
}

void channel_responses(channel * restrict chan, int code, const char * restrict text)
{
  channel_responsew(chan, code, text, strlen(text));
}

void channel_responsew(channel * restrict chan, int code, const char * restrict text, uint16_t text_len)
{
  bamipc_message *msg;

  msg = channel_produce(chan);
  msg->id = chan->msgid;
  msg->size = text_len;
  msg->type = BAMIPC_MSG_RESPONSE;
  msg->code = code;
  if(text_len) {
    memcpy(msg->text, text, text_len);
  }

  channel_post(chan, msg);
}

void channel_setup()
{
}

void channel_cleanup()
{
}

void channel_reconfigure(config * restrict cfg, bool dry)
{
  /* there should be configuration on the max number of possible clients/channels */
  return;
}
