/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>


#define STDOUT_SAVED_REAL  601
#define STDERR_SAVED_REAL  602

/* non-configurable execution parameters */
extern struct g_params {
  pid_t           pid;              // process id of this process
  pid_t           thread_monitor;   // thread id of the monitor thread
  pid_t           thread_server;    // thread id of the server thread
//  pid_t           thread_reconcile; // thread id of the reconcile thread
  pid_t           thread_notify;    // thread id of the notify thread
  pid_t           thread_sweeper;   // thread id of the sweeper thread
  pid_t           thread_beholder;  // thread id of the beholder thread
//  pid_t           thread_build;     // thread id of the build thread
  pid_t           thread_run;       // thread id of the run thread
  int             thread_count;     // number of extant threads, other than the monitor/main thread

  struct timespec starttime;

  long            procs;            // detected number of processors (0 means unknown)

  char *          ipcdir;
  char *          homedir;          // homedir of the real user
  char *          proj_dir;         // absolute path of the project directory
  int             proj_dirfd;       // readonly file descriptor for the project dir

  int             shutdown;         // whether shutdown has been initiated
  bool            handler_error;    // error on any handler thread
} g_params;

/// params_parse
//
// SUMMARY
//  populates g_params
//
void params_setup(uint64_t hash);

/// params_teardown
//
// free g_params
//
void params_cleanup(void);
