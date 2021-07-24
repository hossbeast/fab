/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include "xlinux/xshm.h"
#include "fab/ipc.h"

#include "channel.h"
#include "config.internal.h"
#include "params.h"

/* ugo+rw  world read/write */
#define SHMCHANNEL_MODE 0666

#include <inttypes.h>
#include "fab/events.h"
#include "common/attrs.h"

//
// public
//

void channel_create(channel ** restrict chanp, pid_t thread_id)
{
  channel *chan;
  int shmid = -1;
  void * shmaddr = 0;

  shmid = xshmget(IPC_PRIVATE, FABIPC_SHMSIZE, IPC_CREAT | IPC_EXCL | SHMCHANNEL_MODE);
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

fabipc_message * channel_produce(channel * restrict chan)
{
  fabipc_message *msg;
  msg = fabipc_produce(
      chan->ipc.server_ring.pages
    , &chan->ipc.server_ring.head
    , &chan->ipc.server_ring.tail
    , FABIPC_SERVER_RINGSIZE - 1
  );

  msg->id = chan->msgid;

  return msg;
}

void channel_post(channel * restrict chan, fabipc_message * restrict msg)
{
#if DEBUG || DEVEL
  RUNTIME_ASSERT(msg->type);
  if(msg->type == FABIPC_MSG_EVENTS) {
    RUNTIME_ASSERT(msg->evtype);
  }
#endif

  if(msg->type == FABIPC_MSG_RESPONSE && msg->code != 0)
  {
    chan->error = true;
    write(2, msg->text, msg->size);
  }

#if 0
  uint32_t h;
  char buf[64];
  size_t z;

  h = containerof(msg, fabipc_page, msg)->tail;
  z = znload_attrs32(buf, sizeof(buf), fabipc_msg_type_attrs, msg->type);
  printf("tx tail %5u id 0x%016"PRIx64" code %8d type %.*s", h, msg->id, msg->code, (int)z, buf);
  if(msg->type == FABIPC_MSG_EVENTS)
  {
    z = znload_attrs32(buf, sizeof(buf), fabipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu", msg->size);
  if(msg->type == FABIPC_MSG_RESPONSE || (msg->type == FABIPC_MSG_EVENTS && msg->evtype == FABIPC_EVENT_SYSTEM_STATE))
  {
    printf(" %.*s", (int)msg->size, msg->text);
  }
  printf("\n");
#endif

  fabipc_post(msg, &chan->ipc.server_ring.waiters);
}

/* RX : acquire -> consume */

fabipc_message * channel_acquire(channel * restrict chan)
{
  fabipc_message *msg;

  msg = fabipc_acquire(
      chan->ipc.client_ring.pages
    , &chan->ipc.client_ring.head
    , &chan->ipc.client_ring.tail
    , FABIPC_CLIENT_RINGSIZE - 1
  );

#if 0
  if(!msg) { return msg; }

  uint32_t h;
  char buf[64];
  size_t z;

  h = containerof(msg, fabipc_page, msg)->head;
  z = znload_attrs32(buf, sizeof(buf), fabipc_msg_type_attrs, msg->type);
  printf("rx head %5u id 0x%016"PRIx64" code %8d type %.*s", h, msg->id, msg->code, (int)z, buf);
  if(msg->type == FABIPC_MSG_EVENTS)
  {
    z = znload_attrs32(buf, sizeof(buf), fabipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu\n", msg->size);
  if(msg->type == FABIPC_MSG_REQUEST)
  {
    printf(msg->text, msg->size);
    printf("\n");
  }
#endif

#if DEBUG || DEVEL
  if (msg) {
    RUNTIME_ASSERT(msg->type);
  }
#endif

  return msg;
}

void channel_consume(channel * restrict chan, fabipc_message * restrict msg)
{
  fabipc_consume(
      chan->ipc.client_ring.pages
    , &chan->ipc.client_ring.head
    , msg
    , FABIPC_CLIENT_RINGSIZE - 1
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
  fabipc_message *msg;

  msg = channel_produce(chan);
  msg->id = chan->msgid;
  msg->size = text_len;
  msg->type = FABIPC_MSG_RESPONSE;
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

void channel_reconfigure(configblob * restrict cfg, bool dry)
{
  if(dry) {
    return;
  }

  if(!(cfg->channels.attrs & CONFIG_CHANGED)) {
    return;
  }

  /* there should be configuration on the max number of possible clients/channels */
}
