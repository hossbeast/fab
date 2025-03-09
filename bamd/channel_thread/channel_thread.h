/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

There is one handler thread per client

*/

#include "types.h"
#include "rcu_list.h"
#include "channel_thread.h"
#include "channel.h"

extern rcu_list channel_thread_list;

typedef struct channel_thread {
  rcu_list stk;   // channel_thread_list
  pid_t tid;      /* for receiving signals from the run thread */

  /* subscribed events */
  uint32_t event_mask;

  /* waiting for run thread to complete */
  bool run_thread_pending;

  /* bamipc channel for the client, if any */
  struct channel *channel;
} channel_thread;

/*
 * launch a channel handler thread
 *
 * client_pid - pid of the client, if any
 * client_tid - thread id of the client, if any
 */
void channel_thread_launch(pid_t client_pid, pid_t client_tid)
  __attribute__((nonnull));
