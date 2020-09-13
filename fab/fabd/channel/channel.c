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

#include "types.h"
#include "xapi.h"

#include "fab/ipc.h"
#include "valyria/llist.h"
#include "xlinux/xshm.h"
#include "xlinux/xfutex.h"

#include "channel.h"
#include "config.internal.h"
#include "params.h"

#include "common/attrs.h"

#include "atomics.h"

static llist freelist = LLIST_INITIALIZER(freelist);

//
// public
//

xapi channel_create(fabipc_channel ** restrict chanp, pid_t thread_id)
{
  enter;

  fabipc_channel *chan;
  int shmid = -1;
  void * shmaddr = 0;

  fatal(xshmget, IPC_PRIVATE, FABIPC_SHMSIZE, IPC_CREAT | IPC_EXCL | FABIPC_MODE_DATA, &shmid);
  fatal(xshmat, shmid, 0, 0, &shmaddr);

  chan = shmaddr;
  chan->shmid = shmid;
  chan->server_pid = g_params.pid;
  chan->server_tid = thread_id;
  chan->server_pulse = thread_id;
  *chanp = chan;

finally:
  if(shmid != -1) {
    fatal(xshmctl, shmid, IPC_RMID, 0);
  }
coda;
}

xapi channel_xfree(fabipc_channel * restrict chan)
{
  enter;

  if(chan)
  {
    fatal(xshmdt, chan);
  }

  finally : coda;
}

xapi channel_setup()
{
  enter;

  finally : coda;
}

xapi channel_cleanup()
{
  enter;

  finally : coda;
}

xapi channel_reconfigure(config * restrict cfg, bool dry)
{
  enter;

  if(dry)
    goto XAPI_FINALIZE;

  if(!(cfg->channels.attrs & CONFIG_CHANGED))
    goto XAPI_FINALIZE;

  finally : coda;
}

fabipc_message * channel_produce(fabipc_channel * restrict chan, uint32_t * restrict tail_next, size_t sz)
{
  fabipc_message *msg;

  msg = fabipc_produce(
      chan->server_ring.pages
    , &chan->server_ring.head
    , &chan->server_ring.tail
    , tail_next
    , &chan->server_ring.overflow
    , FABIPC_SERVER_RINGSIZE - 1
    , sz
  );

  if(msg == 0) {
    return 0;
  }

//printf("server: %7s server ring head %4u tail %4u msg %4u\n", "produce", chan->server_ring.head, chan->server_ring.tail, msg->id);

  return msg;
}

void channel_post(fabipc_channel * restrict chan, uint32_t tail_next)
{
//  uint32_t tail;
//  uint32_t index;
//  fabipc_page *page;
//  fabipc_message *msg;
//
//  tail = chan->server_ring.tail;
//  index = tail & (FABIPC_SERVER_RINGSIZE - 1);
//  page = &chan->server_ring.pages[index];
//  msg = &page->msg;

//printf("server: %7s server ring head %4u tail %4u msg %4u type %s size %u text %.*s\n"
//  , "post"
//  , chan->server_ring.head
//  , chan->server_ring.tail
//  , msg->id
//  , attrs32_name_byvalue(fabipc_msg_type_attrs, FABIPC_MSG_TYPE_OPT & msg->type)
//  , msg->size
//  , (int)msg->size
//  , msg->text
//);

  fabipc_post(&chan->server_ring.tail, tail_next, &chan->server_ring.waiters);
}

fabipc_message * channel_acquire(fabipc_channel * restrict chan)
{
  fabipc_message *msg;

  msg = fabipc_acquire(
      chan->client_ring.pages
    , &chan->client_ring.head
    , &chan->client_ring.tail
    , FABIPC_CLIENT_RINGSIZE - 1
  );

  if(msg == 0) {
    return 0;
  }


//printf("server: %7s client ring head %4u tail %4u msg %4u type %s size %u\n"
//  , "acquire"
//  , chan->client_ring.head
//  , chan->client_ring.tail
//  , msg->id
//  , attrs32_name_byvalue(fabipc_msg_type_attrs, FABIPC_MSG_TYPE_OPT & msg->type)
//  , msg->size
//);

#if 0
printf("server: %7s msg at client ring index %u size %u ", "acquire", index, msg->size);
int x;
for(x = 0; x < 8; x++) {
  if(x >= msg->size) {
    break;
  }
  printf("%02hx", msg->text[x]);
}
printf("\n");
#endif

  return msg;
}

void channel_consume(fabipc_channel * restrict chan)
{
//printf("server: %7s client ring head %4u tail %4u\n"
//  , "consume"
//  , chan->client_ring.head
//  , chan->client_ring.tail
//);

  fabipc_consume(&chan->client_ring.head);
}

#if 0

channel_msg * channel_msg_alloc(channel * restrict chan, size_t sz)
{
  uint32_t head;
  uint32_t tail;
  uint32_t next;
  uint32_t index;
  struct channel_entry *entry;

  RUNTIME_ASSERT(sz <= (sizeof(channel_entry) - sizeof(channel_msg)));

  head = chan->server_ring.head;
  tail = chan->server_ring.tail;
  smp_rmb();
  next = tail + 1;
  if(next == head) {
    chan->server_ring.overflow = 1;
    return NULL;
  }

  index = tail & (CHANNEL_SERVER_RINGSIZE - 1);
  entry = &chan->server_ring.entries[index];

printf("server: cx msg at server ring index %u size %zu\n", index, sz);

  return &entry->msg;
}

void channel_msg_post(channel * restrict chan)
{
  uint32_t tail;
  uint32_t index;
  int32_t one = 1;
  int32_t zero = 0;

  tail = chan->server_ring.tail;
  index = tail & (CHANNEL_SERVER_RINGSIZE - 1);

  chan->server_pulse++;
  chan->server_ring.tail++;

bool woken = false;
  if(atomic_cas_i32(&chan->server_ring.waiters, &one, &zero)) {
    smp_wmb();
    syscall(SYS_futex, FUTEX_WAKE, 1, 0, 0, 0);
woken = true;
  }

printf("server: tx msg on server ring index %u%s\n", index, woken ? " [*]" : "");

//  fatal(sigutil_uxtgkill, &r, chan->client_pid, chan->client_tid, SIGUSR1);
}

channel_msg * channel_msg_acquire(channel * restrict chan)
{
  uint32_t head;
  uint32_t index;
  struct channel_entry *entry;

  head = chan->client_ring.head;
  smp_rmb();
  if(head == chan->client_ring.tail) {
    return NULL;  // ring is empty
  }

  index = head & (CHANNEL_CLIENT_RINGSIZE - 1);
  entry = &chan->client_ring.entries[index];

printf("server: rx msg at client ring index %u\n", index);

  return &entry->msg;
}

void channel_msg_release(channel * restrict chan)
{
  chan->client_ring.head++;
  smp_wmb();
}
#endif
