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

#ifndef _PARAMS_H
#define _PARAMS_H

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

#endif
