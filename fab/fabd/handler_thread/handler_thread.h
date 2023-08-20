/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

There is one handler thread per client

*/

#include "types.h"
#include "locks.h"

extern struct futexlock handlers_lock;

/*
 * launch a handler thread
 *
 * client_pid - pid of the client, if any
 * client_tid - thread id of the client, if any
 */
void handler_thread_launch(pid_t client_pid, pid_t client_tid)
  __attribute__((nonnull));
