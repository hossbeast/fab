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

#ifndef _FABCORE_IPC_H
#define _FABCORE_IPC_H

#include "xapi.h"

#if 0
//
// signals used for communication between fab processes start at FABSIG_BASE
//
#define FABSIG_BASE 40

/// FABSIG_READY
//
// fabd -> fab
//  ready to accept commands
//
#define FABSIG_READY FABSIG_BASE + 0

/// FABSIG_START
//
// fab  -> fabd
//  new command is ready to be executed
//
#define FABSIG_START FABSIG_BASE + 1

/// FABSIG_DONE
//
// fabd -> fab
//  the command is complete
//
#define FABSIG_DONE FABSIG_BASE + 2

/// FABSIG_RECVD
//
// fab -> fabd
//  response received
//
#define FABSIG_RECVD FABSIG_BASE + 3

/// FABSIG_BPSTART
//
// fabd -> faba
//  buildplan stage is ready to be executed
//
#define FABSIG_BPSTART FABSIG_BASE + 4

/// FABSIG_BPGOOD
//
// faba -> fabd 
//  buildplan was executed successfully
//
#define FABSIG_BPGOOD FABSIG_BASE + 5

/// FABSIG_BPBAD
//
// faba -> fabd
//  buildplan was not successfully executed to completion
//
#define FABSIG_BPBAD FABSIG_BASE + 6

/// FABSIG_SIZE
//
// number of fab ipc signals, used for enumerating
//
#define FABSIG_SIZE 7
#endif

#define FABSIG_SYN  SIGUSR1
#define FABSIG_ACK  SIGUSR2

/// FABIPC_DATA
//
// LOCATION
//  {FABIPCDIR,FABTCMPDIR}/*   files which need not be executable
//
// DESCRIPTION
//  ugo+rw   world read/write
//
// RATIONALE
//
#define FABIPC_DATA 0666

/// FABIPC_CODE
//
// LOCATION
//  {FABIPCDIR,FABTCMPDIR}/*    files which need to be executable
//
// DESCRIPTION
//  ugo+rwx   world read/write/exec
//
// RATIONALE
//  
#define FABIPC_CODE 0777

/// FABIPC_DIR
//
// LOCATION
//  {FABIPCDIR,FABTCMPDIR}/*    directories
//
// DESCRIPTION
//  ugo+rwx   world read/write/exec
//
// RATIONALE
//  
#define FABIPC_DIR  0777

#define FABIPC_REQUEST  0x13
#define FABIPC_RESPONSE 0x7c

#define restrict __restrict

/// ipc_lock_obtain
//
// SUMMARY
//  attempt to obtain an exclusive lock
//
// REMARKS
//  ipcdir - 
//  pgid   - (returns) zero on success, otherwise pgid of lock holder
//
xapi ipc_lock_obtain(pid_t * pgid, char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2)));

xapi ipc_lock_update(char * const restrict fmt, ...)
  __attribute__((nonnull(1)));

xapi ipc_lock_release(char * const restrict fmt, ...)
  __attribute__((nonnull(1)));

#undef restrict
#endif
