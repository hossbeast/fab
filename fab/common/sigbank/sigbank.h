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

#ifndef _SIGBANK_H
#define _SIGBANK_H

/*

 sigbank enables a signal handling pattern where application signals are handled
 at specified signal-exchange points and system signals are always unblocked

 this allows for blocking system calls to be interrupted by the receipt of system
 signals, while maintaining well-defined semantics for two processes to exchange
 application signals

 the applicaton signals are defined in params.h

*/

#define restrict __restrict

// the signal handler writes to these variables and nothing more
extern volatile int g_signum;
extern volatile pid_t g_sigpid;

/// sigbank_init
//
// SUMMARY
//  cleanup
//
// PARAMETERS
//  name - name to use in ipc/debug logging
//  pid  - name to use in ipc/debug logging
//
// RETURNS
//  xapi semantics
//
int sigbank_init(
#if DEBUG_IPC
    char * name
  , pid_t pid
#endif
);

/// sigbank_teardown
//
// SUMMARY
//  cleanup
//
void sigbank_teardown();

/// sigreceive
//
// SUMMARY
//  suspends execution pending receipt of some signal. If the signal received is an application signal
//  then the signal and pid of the sender are returned if those parameters are not null
//
// PARAMETERS
//  [expsig] - sentinel-terminated list of expected application signals
//  [exppid] - expected pid of application signal sender
//  [actsig] - (returns) received application signal
//  [actpid] - (returns) pid of sender of application signal
//
// REMARKS
//  this api is used to receive application signals ; system signals can be
//  recived at any time and should be received via g_signum directly
//
// RETURNS
//  xapi semantics
//
int sigreceive(int * restrict expsig, int exppid, int * restrict actsig, pid_t * restrict actpid);

/// sigexchange
//
// SUMMARY
//  call xkill with the specified parameters then suspend exuection pending signal receipt
//  optionally validate the received signal, optionally return the received signal and pid of sender
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
// RETURNS
//  xapi semantics
//
int sigexchange(pid_t pid, int sig, int * restrict expsig, int exppid, int * restrict actsig, pid_t * restrict actpid);

#undef restrict
#endif
