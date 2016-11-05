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

#ifndef _FAB_SIGBANK_H
#define _FAB_SIGBANK_H

/*

 sigbank enables a signal handling pattern where application signals are handled
 at specified signal-exchange points and system signals are always unblocked

 this allows for blocking system calls to be interrupted by the receipt of system
 signals, while maintaining well-defined semantics for two processes to exchange
 application signals

*/

#include <signal.h>

#include "xapi.h"

#define restrict __restrict

/// sigbank_setup
//
// SUMMARY
//  initialize sigbank and establish process-wide signal handling disposition
//
// PARAMETERS
//  sigmin - least application signal
//  sigmax - greatest application signal
//  [name] - name to use in IPC logging
//
xapi sigbank_setup(const char * restrict name);

/// sigbank_teardown
//
// SUMMARY
//  cleanup
//
void sigbank_teardown(void);

/// sigbank_receive
//
// SUMMARY
//  uses sigsuspend to temporarily unblock all signals and suspend execution, pending receipt of
//  any signal. If the received signal is an application signal, and expsig is provided, then the
//  actual signal received must be in the set specified by expsig, otherwise FAB_BADIPC is raised
//
// PARAMETERS
//  [expsig] - sentinel-terminated list of expected application signals
//  [exppid] - expected pid of application signal sender
//  [actsig] - (returns) received application signal
//  [actpid] - (returns) pid of sender of application signal
//
// ERRORS 
//  FAB_BADIPC - an application signal was received, expsig was not null, and the received signal was not in the set
//               or exppid was not null, and the actual sender was not exppid
//
// RETURNS
//  true if a signal was received, false otherwise
//
xapi sigbank_receive(int * const restrict rsig, pid_t * const restrict rpid);

/// sigbank_exchange
//
// SUMMARY
//  call xkill with the specified parameters then call sigreceive with the specified parameters
//
// PARAMETERS
//  pid      - see xkill
//  sig      - see xkill
//  [expsig] - sentinel-terminated list of expected application signals
//  [exppid] - expected pid of application signal sender
//  [actsig] - (returns) received application signal
//  [actpid] - (returns) pid of sender of application signal
//
// REMARKS
//  used to initiate signal-exchange (thus, only by fab and faba, never by fabd)
//
xapi sigbank_exchange(int sig, pid_t pid, int * const restrict rsig, pid_t * const restrict rpid);

/// sigbank_assert
//
// SUMMARY
//  
//
// PARAMETERS
//
xapi sigbank_assert(int expsig, int actsig, pid_t exppid, pid_t actpid);

#undef restrict
#endif
