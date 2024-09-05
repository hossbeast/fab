/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>

#include "types.h"
#include "macros.h"
#include "barriers.h"
#include "atomics.h"

struct attrs32;

/* allocated size of the channel */
#define BAMIPC_SHMSIZE          (32 * 1024 * 1024)
#define BAMIPC_PAGESIZE         16304
#define BAMIPC_RINGSIZE         1024

/* max number of concurrent producers without stalling */
#define BAMIPC_PRODUCERS        16

#define BAMIPC_MSG_TYPE_OPT 0xff

#define BAMIPC_MSG_TYPE_TABLE                                                                                  \
  DEF(BAMIPC_MSG_REQUEST   , "request"  , BAMIPC_MSG_TYPE_OPT, 0x01)  /* request, a bacon-formatted string */  \
  DEF(BAMIPC_MSG_RESPONSE  , "response" , BAMIPC_MSG_TYPE_OPT, 0x02)  /* end of processing this request */     \
  DEF(BAMIPC_MSG_RESULT    , "result"   , BAMIPC_MSG_TYPE_OPT, 0x03)  /* result of processing this request */  \
  DEF(BAMIPC_MSG_EVENTSUB  , "eventsub" , BAMIPC_MSG_TYPE_OPT, 0x06)  /* subscribe to events (additive) */     \
  DEF(BAMIPC_MSG_EVENTS    , "events"   , BAMIPC_MSG_TYPE_OPT, 0x07)  /* events */                             \

typedef enum bamipc_msg_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
BAMIPC_MSG_TYPE_TABLE
#undef DEF
} bamipc_msg_type;

extern struct attrs32 * bamipc_msg_type_attrs;

#define BAMIPC_MESSAGE_FIELDS                               \
  char _pad16[16];  /* application reserved space */        \
  bamipc_msg_type type;                                     \
  int32_t code;                                             \
  uint64_t id;                                              \
  union {                                                   \
    uint32_t attrs;                                         \
    uint32_t evtype;                                        \
  };                                                        \
  uint32_t size;              /* size of the text field */  \

enum bamipc_page_state {
  BAMIPC_PAGE_STATE_UNUSED,
  BAMIPC_PAGE_STATE_POSTED,
  BAMIPC_PAGE_STATE_ACQUIRED,
};

/* calculate the offset to text */
struct _bamipc_page_layout {
  enum bamipc_page_state state;
  uint32_t head;
  struct __attribute__((aligned(8))) {
    BAMIPC_MESSAGE_FIELDS
  } msg;
  char __attribute__((aligned(8))) text[];
};

typedef struct bamipc_message {
  BAMIPC_MESSAGE_FIELDS
  char __attribute__((aligned(8))) text[BAMIPC_PAGESIZE - offsetof(struct _bamipc_page_layout, text)];
} bamipc_message;

typedef struct bamipc_page {
  union {
    struct {
      enum bamipc_page_state state;
      union {
        uint32_t head;
        uint32_t tail;  // not needed - debug only
      };
      bamipc_message msg;
    };
    char pad[BAMIPC_PAGESIZE];
  };
} bamipc_page;
STATIC_ASSERT(sizeof(bamipc_page) == BAMIPC_PAGESIZE);

/*
 * Channels are created and owned by bamd (server). The channel consists of a multi-producer, single-consumer
 * ring in each direction.
 */
typedef struct bamipc_channel
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
    struct bamipc_page pages[BAMIPC_RINGSIZE];
    uint32_t head;          // updated by server when message(s) consumed
    uint32_t tail;          // updated by client when message(s) posted
    int32_t __attribute__((aligned(4))) waiters;
    uint32_t producers;     // active producers
  } client_ring;

  // messages server -> client
  struct __attribute__((aligned(8))) {
    struct bamipc_page pages[BAMIPC_RINGSIZE];
    uint32_t head;          // updated by client when message(s) consumed
    uint32_t tail;          // updated by server when message(s) posted
    int32_t __attribute__((aligned(4))) waiters;
    uint32_t producers;     // active producers
  } server_ring;
} bamipc_channel;

STATIC_ASSERT(sizeof(bamipc_channel) <= BAMIPC_SHMSIZE);

/*
 * Prepare to use the next message at the ring tail. Messages are not visible
 * until posted. Multiple producers are supported.
 *
 * pages      - pointer to the ring pages
 * ring_head  - pointer to ring head
 * ring_tail  - pointer to ring tail
 * producers  - pointer to ring producers
 * waiters    - pointer to ring waiters
 */
bamipc_message * bamipc_produce(
    bamipc_page * restrict pages
  , uint32_t * restrict ring_head
  , uint32_t * restrict ring_tail
  , uint32_t * restrict producers
  , int32_t * restrict waiters
)
  __attribute__((nonnull));

/*
 * Post previously produced messages to the ring.
 *
 * msg        - message to post
 * producers  - pointer to ring producers
 * waiters    - pointer to ring waiters
 */
void bamipc_post(
    bamipc_message * restrict msg
  , uint32_t * restrict producers
  , int32_t * restrict waiters
)
  __attribute__((nonnull));

/*
 * Get the next available message at the ring head. There may be only one consumer.
 *
 * pages      - pointer to ring pages
 * ring_head  - pointer to ring head
 * ring_tail  - pointer to ring tail
 */
bamipc_message * bamipc_acquire(
    bamipc_page * restrict pages
  , uint32_t * restrict ring_head
  , uint32_t * restrict ring_tail
)
  __attribute__((nonnull));

/*
 * Release one previously acquired message.
 *
 * pages      - pointer to ring pages
 * ring_head  - pointer to ring head
 * msg        - message to release
 * waiters    - pointer to ring waiters
 */
void bamipc_consume(
    bamipc_page * restrict pages
  , uint32_t * restrict ring_head
  , bamipc_message * restrict msg
  , int32_t * restrict waiters
)
  __attribute__((nonnull));

/*
 * Release acquired messages up to and including the specified message.
 *
 * pages      - pointer to ring pages
 * ring_head  - pointer to ring head
 * msg        - message at the end of the batch
 */
void bamipc_release(
    bamipc_page * restrict pages
  , uint32_t * restrict ring_head
  , bamipc_message * restrict msg
)
  __attribute__((nonnull));
