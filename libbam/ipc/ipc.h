/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>

#include "common/attrs.h"

#include "types.h"
#include "macros.h"
#include "barriers.h"
#include "atomics.h"

struct attrs32;
struct timespec;

/* size of message payloads, should be page-aligned */
#define BAMIPC_PAGESIZE 16384

/* number of messages in the ring */
#define BAMIPC_RINGSIZE 512

/* number of pages available to use in the ring */
#define BAMIPC_NUMPAGES 600

#define BAMIPC_MSG_TYPE_OPT 0xff
#define BAMIPC_MSG_TYPE_TABLE                                                                                  \
  DEF(BAMIPC_MSG_PING      , "ping"     , BAMIPC_MSG_TYPE_OPT, 0x02)  /* still alive? */                       \
  DEF(BAMIPC_MSG_PONG      , "pong"     , BAMIPC_MSG_TYPE_OPT, 0x03)  /* still alive */                        \
  DEF(BAMIPC_MSG_REQUEST   , "request"  , BAMIPC_MSG_TYPE_OPT, 0x04)  /* request, a bacon-formatted string */  \
  DEF(BAMIPC_MSG_RESPONSE  , "response" , BAMIPC_MSG_TYPE_OPT, 0x05)  /* end of processing this request */     \
  DEF(BAMIPC_MSG_RESULT    , "result"   , BAMIPC_MSG_TYPE_OPT, 0x06)  /* result of processing this request */  \
  DEF(BAMIPC_MSG_EVENTSUB  , "eventsub" , BAMIPC_MSG_TYPE_OPT, 0x07)  /* subscribe to events (additive) */     \
  DEF(BAMIPC_MSG_EVENTS    , "events"   , BAMIPC_MSG_TYPE_OPT, 0x08)  /* events */                             \

typedef enum bamipc_msg_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
BAMIPC_MSG_TYPE_TABLE
#undef DEF
} bamipc_msg_type;

extern struct attrs32 bamipc_msg_type_attrs;

#define BAMIPC_MSG_FORMAT_OPT 0xff
#define BAMIPC_MSG_FORMAT_TABLE                                                                                \
  DEF(BAMIPC_MSG_TEXT       , "text/plain"      , BAMIPC_MSG_FORMAT_OPT, 0x03)  /* ascii text */               \
  DEF(BAMIPC_MSG_BACON      , "text/bacon"      , BAMIPC_MSG_FORMAT_OPT, 0x02)  /* bacon-valued ascii text  */ \

typedef enum bamipc_msg_format {
#undef DEF
#define DEF(x, s, r, y) x = y,
BAMIPC_MSG_FORMAT_TABLE
#undef DEF
} bamipc_msg_format;

extern struct attrs32 bamipc_msg_format_attrs;


#define BAMIPC_MSG_CODE_OPT 0xff
#define BAMIPC_MSG_CODE_TABLE                                                                                  \
  DEF(BAMIPC_MSG_SUCCESS  , "SUCCESS" , BAMIPC_MSG_CODE_OPT, 0x00)    /* success */                            \
  DEF(BAMIPC_MSG_FAILURE  , "FAILURE" , BAMIPC_MSG_CODE_OPT, 0x01)    /* unspecified failure */                \
  DEF(BAMIPC_MSG_ENOENT   , "ENOENT"  , BAMIPC_MSG_CODE_OPT, 0x02)    /* entity not found */                   \

enum {
#undef DEF
#define DEF(x, s, r, y) x = y,
BAMIPC_MSG_CODE_TABLE
#undef DEF
};

extern struct attrs32 bamipc_msg_code_attrs;

#define BAMIPC_COUNTER_TABLE                                                     \
  DEF(BAMIPC_COUNTER_TX_FAST       , "tx-fast"         , 0x0f, 0x01)             \
  DEF(BAMIPC_COUNTER_TX_WAIT       , "tx-wait"         , 0x0f, 0x02)             \
  DEF(BAMIPC_COUNTER_TX_STALL      , "tx-stall"        , 0x0f, 0x03)             \
  DEF(BAMIPC_COUNTER_PUBLISH_FAST  , "publish-fast"    , 0xf0, 0x10)             \
  DEF(BAMIPC_COUNTER_PUBLISH_STALL , "publish-stall"   , 0xf0, 0x20)             \

enum {
#undef DEF
#define DEF(x, s, r, y) x = y,
BAMIPC_COUNTER_TABLE
#undef DEF
};

extern struct attrs32 bamipc_counter_attrs;

typedef struct bamipc_message {
  /* application space */
  char _pad0[8];
  uint32_t flags;
  bamipc_msg_type type;
  int32_t code;
  uint64_t id;
  uint32_t format;    // bampipc_msg_format / descriptor id
  union {
    uint32_t attrs;
    uint32_t evtype;  // bamipc_event_type / BAMIPC_EVENT_*
  };
  uint32_t slot;
  uint32_t index;   /* this field is only used in unit tests */
  char *text;
  uint32_t size;    /* size of buffer pointed to by text */
  uint32_t len;     /* portion of text in use */
} bamipc_message;

typedef struct bamipc_page {
  char text[BAMIPC_PAGESIZE];
} bamipc_page;

/*
 * multi-producer, single-consumer, unidirectional
 *
 * tx at the tail, produce -> reserve -> post
 * rx at the head, acquire -> consume
 */
typedef struct bamipc_ring {
  uint32_t prod;    /* producers synchronize on prod to allocate tail slots */
  uint32_t tail;    /* produce at tail */
  uint32_t head;    /* consume at head */
} bamipc_ring;

/*
 * multi-producer, single-consumer, unidirectional
 *
 * tx -> publish
 * rx -> release
 */
typedef struct bamipc_pipe {
  bamipc_ring ring;
  struct bamipc_message messages[BAMIPC_RINGSIZE];
  struct bamipc_page pages[BAMIPC_NUMPAGES];

  /* counters */
  struct {
    /* tx */
    uint32_t tx_fast;     /* uncontended */
    uint32_t tx_waits;    /* kernel-mediated wait */
    uint32_t tx_stalls;   /* spurious wait */
    /* post */
    uint32_t publish_fast;    /* uncontended */
    uint32_t publish_stalls;  /* contended */
  } stats;
} bamipc_pipe;

/*
 * Channels are created and owned by bamd (server).
 * A channel consists of an rx and a tx pipe for bidirectional communication.
 */
typedef struct bamipc_channel {
  int shmid;

  /* populated before client attachment, immutable thereafter */
  char client_id[40];
  uint16_t client_id_len;

  pid_t client_pid;
  pid_t client_tid;
  bool client_exit;       // set to true by the client to affirmatively close the tx ring

  pid_t server_pid;
  pid_t server_tid;
  bool server_exit;       // set to true by the server to affirmatively close the rx ring

  // tx. messages client -> server
  bamipc_pipe tx;

  // rx. messages client <- server
  bamipc_pipe rx;
} bamipc_channel;

#define BAMIPC_OK         0
#define BAMIPC_STALLED    1
#define BAMIPC_AWOKEN     2
#define BAMIPC_WAITED     3
#define BAMIPC_ETIMEDOUT  4
#define BAMIPC_EINTR      5
#define BAMIPC_EAGAIN     6

void bamipc_channel_init(struct bamipc_channel * restrict channel)
  __attribute__((nonnull));

void bamipc_channel_reset(struct bamipc_channel * restrict channel)
  __attribute__((nonnull));
