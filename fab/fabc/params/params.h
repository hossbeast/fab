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

#include "xapi.h"

// non-configurable execution parameters
extern struct g_params
{
  pid_t           pid;     // pid of this process
  pid_t           ppid;    // pid of parent process
  pid_t           pgid;    // process group id

  pid_t           thread_main;
  pid_t           thread_ui;
  pid_t           thread_client;
  int             thread_count;
  bool            shutdown;

  char *          exedir;  // path to the directory containing the executable
} g_params;

/// params_parse
//
// SUMMARY
//  parses command-line options, populating g_params.
//
xapi params_setup(void);

/// params_teardown
//
// free g_params
//
void params_teardown(void);

/// params_report
//
// SUMMARY
//  log a description of params
//
xapi params_report(void);

#endif
