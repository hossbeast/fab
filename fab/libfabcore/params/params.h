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

#ifndef _FABCORE_PARAMS_H
#define _FABCORE_PARAMS_H

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

extern struct g_params_t
{
  //
  // non-configurable execution parameters
  //

  pid_t           pid;                        // pid of this process
  pid_t           ppid;                       // pid of parent process
  pid_t           pgid;                       // process group id

  uid_t           ruid;                       // real-user-id
  char *          ruid_name;
  int             ruid_namel;
  uid_t           euid;                       // effective-user-id   (fabsys in the final build)
  char *          euid_name;
  int             euid_namel;
  gid_t           rgid;                       // real-group-id
  char *          rgid_name;
  int             rgid_namel;
  gid_t           egid;                       // effective-group-id  (fabsys in the final build)
  char *          egid_name;
  int             egid_namel;
  char *          cwd;                        // cwd
  int             cwdl;
  char *          exedir;                     // location of the binary executable
  int             exedirl;
  long            procs;                      // detected number of processors (0 means unknown)
  long            pagesize;                   // page size

//pid_t           fab_pid;                    // pid of the fab process
//pid_t           fabd_pgid;                  // pgid of the fabd process
//time_t          starttime;                  // time last action of fabd began
//uint32_t        canhash;                    // ipc hash
//char            canhashs[32];               // ipc hash as string
//char            ipcdir[2048];               // ipc dir
} g_params;

/// params_report
//
// SUMMARY
//  log a description of params
//
xapi params_report(void);

#endif
