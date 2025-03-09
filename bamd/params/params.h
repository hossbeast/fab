/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "types.h"

/* non-configurable execution parameters */
extern struct g_params {
  pid_t           pid;              // process id of this process
  pid_t           thread_monitor;   // thread id of the monitor thread
  pid_t           thread_channel_server;  // thread id of the channel server thread
  pid_t           thread_notify;    // thread id of the notify thread
  pid_t           thread_sweeper;   // thread id of the sweeper thread
  pid_t           thread_run;       // thread id of the run thread
  pid_t           thread_timeout;   // thread id of the timeout thread
  int             thread_count;     // number of extant threads, other than the monitor/main thread

  struct timespec starttime;
  char            pid_str[32];

  long            procs;            // detected number of processors (0 means unknown)

  char            ipcdir[PATH_MAX];
  uint16_t        ipcdir_len;
  char            homedir[PATH_MAX];     // ENV{HOME}
  uint16_t        homedir_len;

  int             shutdown;         // whether shutdown has been initiated
  bool            handler_error;    // error on any handler thread
} g_params;

/* populates g_params */
void params_setup(void);
