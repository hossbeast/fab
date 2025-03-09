/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

*/

#include "valyria/llist.h"

#include "types.h"

#include "bam/ipc.h"
#include "stats.h"

struct config;
struct bamipc_channel;
struct client;

typedef union stats_channel {
  struct {
    /* tx ring, client -> server */
    uint32_t client_tx;          /* total */
    uint32_t client_tx_fast;     /* uncontended */
    uint32_t client_tx_waits;    /* kernel-mediated sleep */
    uint32_t client_tx_stalls;   /* spurious wait */
    uint32_t client_publish_fast;
    uint32_t client_publish_stalls;

    /* rx ring, server -> client */
    uint32_t server_tx;          /* total */
    uint32_t server_tx_fast;     /* uncontended */
    uint32_t server_tx_waits;    /* kernel-mediated sleep */
    uint32_t server_tx_stalls;   /* spurious wait */
    uint32_t server_publish_fast;
    uint32_t server_publish_stalls;
  };

  stats_record record;
} stats_channel;

typedef struct channel {
  llist lln;
  struct bamipc_channel *ipc;

  union {
    struct client *client;
    char _transitory;
  };

  /* channel enters the fault state on tx overflow */
  bool fault;
  struct bamipc_message fault_msg;  /* the fault msg has no payload */

  stats_channel channel_stats;

  /* id of the most recently received message  */
  uint64_t rx_msgid;

  /* autoinc */
  uint64_t tx_msgid;
} channel;

//STATIC_ASSERT(sizeof(channel) == BAMIPC_SHMSIZE);

/* allocate an ipc channel
 *
 * chanp     - (returns) the channel
 * thread_id - thread id for the handler of this channel
 */
channel *channel_alloc(void);

void channel_free(channel * restrict ctx);

void channel_response(channel * restrict ctx, uint64_t msgid, bool success)
  __attribute__((nonnull));

/* TX */

struct bamipc_message *channel_produce(channel * restrict ctx, uint64_t msgid)
  __attribute__((nonnull));

void channel_post(channel * restrict ctx, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

/* RX */

int channel_rx(channel * restrict ctx, struct timespec * restrict timeout, struct bamipc_message ** restrict msg)
  __attribute__((nonnull(1, 3)));

void channel_release(channel * restrict ctx, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

void channel_ping(channel * restrict ctx, uint64_t msgid)
  __attribute__((nonnull));

void channel_pong(channel * restrict ctx, uint64_t msgid)
  __attribute__((nonnull));


/* module level */

void channel_setup(void);

void channel_cleanup(void);

void channel_reconfigure(struct config * restrict cfg, bool dry)
  __attribute__((nonnull));
