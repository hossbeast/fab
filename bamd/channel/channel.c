/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <errno.h>

#include "xlinux/xshm.h"
#include "xlinux/xstdlib.h"
#include "bam/ipc.h"
#include "bam/pipe.h"
#include "valyria/llist.h"

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
#include "locks.h"
#include "times.h"

static llist freelist = LLIST_INITIALIZER(freelist);
static struct futexlock freelist_lock;

//
// public
//

channel *channel_alloc()
{
  struct channel *channel = 0;
  int shmid = -1;
  void * shmaddr = 0;

  if(!llist_empty(&freelist)) {
    futexlock_acquire(&freelist_lock);
    channel = llist_shift(&freelist, typeof(*channel), lln);
    futexlock_release(&freelist_lock);
  }

  if(channel) {
    ASAN_CLEAN(channel);
  } else {
    xmalloc(&channel, sizeof(*channel));
    llist_init(&channel->lln);
  }

  if(!channel->ipc) {
    shmid = xshmget(IPC_PRIVATE, sizeof(*channel->ipc), IPC_CREAT | IPC_EXCL | SHMCHANNEL_MODE);
    shmaddr = xshmat(shmid, 0, 0);
    channel->ipc = shmaddr;

    xshmctl(shmid, IPC_RMID, 0);
    bamipc_channel_init(channel->ipc);
    channel->ipc->shmid = shmid;
  }

  bamipc_channel_reset(channel->ipc);

  memset(&channel->_transitory, 0, sizeof(*channel) - offsetof(typeof(*channel), _transitory));

  return channel;
}

static void channel_destroy(channel * restrict chan)
{
  shmdt(chan->ipc);
}

void channel_free(struct channel * restrict channel)
{
  if(!channel) {
    return;
  }

  futexlock_acquire(&freelist_lock);
  llist_push(&freelist, &channel->lln);
  futexlock_release(&freelist_lock);

  ASAN_TAINT(channel);
}

/* TX : produce -> post */
bamipc_message *channel_produce(struct channel * restrict channel, uint64_t msgid)
{
  struct timespec timeout;
  bamipc_message *msg;
  int r;

  timeout.tv_sec = 0;
  timeout.tv_nsec = MSEC_AS_NSEC(500);

  switch((r = bamipc_tx(&channel->ipc->rx, &timeout, &msg))) {
    case BAMIPC_ETIMEDOUT:
    case BAMIPC_EINTR:
      channel->fault = true;
      msg = &channel->fault_msg;  /* the fault message has no payload */
      RUNTIME_ASSERT(msg->size == 0);
    case BAMIPC_OK:
      break;
    default:
      RTA;
  }

  msg->id = msgid;
  msg->len = 0;
  return msg;
}

void channel_post(struct channel * restrict channel, bamipc_message * restrict msg)
{
#if CONFIG_DEBUG || CONFIG_DEVEL
  RUNTIME_ASSERT(msg->type);
  if(msg->type == BAMIPC_MSG_EVENTS) {
    RUNTIME_ASSERT(msg->evtype);
  }
#endif

  if(channel->fault) {
    return;
  }

  bamipc_publish(&channel->ipc->rx, msg);
  channel_tx_probe(channel, msg);
}

/* RX : acquire -> consume */
int channel_rx(struct channel * restrict channel, struct timespec * restrict timeout, bamipc_message ** restrict msg)
{
  int r;
  r = bamipc_rx(&channel->ipc->tx, timeout, msg);
  return r;
}

void channel_release(channel * restrict channel, bamipc_message * restrict msg)
{
  bamipc_release(&channel->ipc->tx, msg);
}

void channel_response(channel * restrict channel, uint64_t msgid, bool success)
{
  bamipc_message *msg;

  msg = channel_produce(channel, msgid);
  msg->type = BAMIPC_MSG_RESPONSE;
  msg->code = BAMIPC_MSG_SUCCESS;
  if(!success) {
    msg->code = BAMIPC_MSG_FAILURE;
  }

  channel_post(channel, msg);
}

void channel_ping(struct channel * restrict channel, uint64_t msgid)
{
  bamipc_message *msg;

  msg = channel_produce(channel, msgid);
  msg->type = BAMIPC_MSG_PING;
  channel_post(channel, msg);
}

void channel_pong(struct channel * restrict channel, uint64_t msgid)
{
  bamipc_message *msg;

  msg = channel_produce(channel, msgid);
  msg->type = BAMIPC_MSG_PONG;
  channel_post(channel, msg);
}

void channel_setup()
{
}

void channel_cleanup()
{
  struct channel *channel;

  while((channel = llist_shift(&freelist, typeof(*channel), lln))) {
    ASAN_CLEAN(channel);
    channel_destroy(channel);
    free(channel);
  }
}

void channel_reconfigure(config * restrict cfg, bool dry)
{
  /* there should be some configuration on the max number of possible clients/channels */
  return;
}
