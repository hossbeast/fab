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

#ifndef _FAB_IPC_H
#define _FAB_IPC_H

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>

#include "xapi.h"
#include "types.h"
#include "macros.h"
#include "barriers.h"
#include "atomics.h"

struct attrs32;

/* allocated size of the channel */
#define FABIPC_SHMSIZE          (16 * 1024 * 1024)
#define FABIPC_PAGESIZE         8096
#define FABIPC_CLIENT_RINGSIZE  1024
#define FABIPC_SERVER_RINGSIZE  1024

#define FABIPC_MSG_TYPE_OPT 0xff

#define FABIPC_MSG_TYPE_TABLE                                                                                  \
  DEF(FABIPC_MSG_REQUEST   , "request"  , FABIPC_MSG_TYPE_OPT, 0x01)  /* request, a bacon-formatted string */  \
  DEF(FABIPC_MSG_RESPONSE  , "response" , FABIPC_MSG_TYPE_OPT, 0x02)  /* end of processing this request */     \
  DEF(FABIPC_MSG_RESULT    , "result"   , FABIPC_MSG_TYPE_OPT, 0x03)  /* result of processing this request */  \
  DEF(FABIPC_MSG_EVENTSUB  , "eventsub" , FABIPC_MSG_TYPE_OPT, 0x06)  /* subscribe to events (additive) */     \
  DEF(FABIPC_MSG_EVENTS    , "events"   , FABIPC_MSG_TYPE_OPT, 0x07)  /* events */                             \

typedef enum fabipc_msg_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
FABIPC_MSG_TYPE_TABLE
} fabipc_msg_type;

extern struct attrs32 * fabipc_msg_type_attrs;

#define FABIPC_MESSAGE_FIELDS                               \
  fabipc_msg_type type;                                     \
  uint64_t id;                                              \
  union {                                                   \
    uint32_t attrs;                                         \
    uint32_t evtype;                                        \
  };                                                        \
  int code;                                                 \
  uint32_t size;              /* size of the text field */  \

/* calculate the offset to text */
struct _fabipc_page_layout {
  int state;
  uint32_t head;
  struct __attribute__((aligned(8))) {
    FABIPC_MESSAGE_FIELDS
  } msg;
  char __attribute__((aligned(8))) text[];
};

typedef struct fabipc_message {
  FABIPC_MESSAGE_FIELDS
  char __attribute__((aligned(8))) text[FABIPC_PAGESIZE - offsetof(struct _fabipc_page_layout, text)];
} fabipc_message;

typedef struct fabipc_page {
  union {
    struct {
      int state;
      union {
        uint32_t head;
        uint32_t tail;  // not needed - debug only
      };
      fabipc_message msg;
    };
    char pad[FABIPC_PAGESIZE];
  };
} fabipc_page;
STATIC_ASSERT(sizeof(fabipc_page) == FABIPC_PAGESIZE);

/*
 * Channels are created and owned by fabd (server). The channel consists of a multi-producer, single-consumer
 * ring in each direction.
 */
typedef struct fabipc_channel
{
  int shmid;

  pid_t client_pid;
  pid_t client_tid;
  uint16_t client_pulse;  // incremented periodically by the client to signal liveness
  bool client_exit;       // set to true by the client to affirmatively close the ring

  pid_t server_pid;
  pid_t server_tid;
  uint16_t server_pulse;  // incremented periodically by the client to signal liveness
  bool server_exit;       // set to true by the server to affirmatively close the ring

  // messages client -> server
  struct __attribute__((aligned(8))) {
    struct fabipc_page pages[FABIPC_CLIENT_RINGSIZE];
    uint32_t head;          // updated by server when message(s) consumed
    uint32_t tail;          // updated by client when message(s) posted
    int32_t __attribute__((aligned(4))) waiters;
  } client_ring;

  // messages server -> client
  struct __attribute__((aligned(8))) {
    struct fabipc_page pages[FABIPC_SERVER_RINGSIZE];
    uint32_t head;          // updated by client when message(s) consumed
    uint32_t tail;          // updated by server when message(s) posted
    int32_t __attribute__((aligned(4))) waiters;
  } server_ring;
} fabipc_channel;

STATIC_ASSERT(sizeof(fabipc_channel) <= FABIPC_SHMSIZE);

/*
 * Prepare to use the next message at the ring tail. Messages are not visible
 * until posted. Multiple producers are supported.
 *
 * pages      - pointer to the ring pages
 * ring_head  - pointer to ring head
 * local_tail - local tail counter
 * tail_used  - (returns) local tail value seen
 * mask       - mask based on ring size
 */
fabipc_message * fabipc_produce(
    fabipc_page * restrict pages
  , uint32_t * restrict ring_head
  , uint32_t * restrict ring_tail
  , uint32_t tail_mask
)
  __attribute__((nonnull));

/*
 * Post previously produced messages to the ring.
 *
 * msg     - message to post
 * waiters - pointer to ring waiters
 */
void fabipc_post(
    fabipc_message * restrict msg
  , int32_t * restrict waiters
)
  __attribute__((nonnull));

/*
 * Get the next available message at the ring head. There may be only one consumer.
 *
 * pages      - pointer to ring pages
 * ring_head  - pointer to ring head
 * ring_tail  - pointer to ring tail
 * mask       - mask based on ring size
 */
fabipc_message * fabipc_acquire(
    fabipc_page * restrict pages
  , uint32_t * restrict ring_head
  , uint32_t * restrict ring_tail
  , uint32_t mask
)
  __attribute__((nonnull));

/*
 * Release one previously acquired message.
 *
 * pages      - pointer to ring pages
 * ring_head  - pointer to ring head
 * msg        - message to release
 * mask       - mask based on ring size
 */
void fabipc_consume(
    fabipc_page * restrict pages
  , uint32_t * restrict ring_head
  , fabipc_message * restrict msg
  , uint32_t mask
)
  __attribute__((nonnull));

/*
 * Release acquired messages up to and including the specified message.
 *
 * pages      - pointer to ring pages
 * ring_head  - pointer to ring head
 * msg        - message at the end of the batch
 * mask       - mask based on ring size
 */
void fabipc_release(
    fabipc_page * restrict pages
  , uint32_t * restrict ring_head
  , fabipc_message * restrict msg
  , uint32_t mask
)
  __attribute__((nonnull));

#endif
