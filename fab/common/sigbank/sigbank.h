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

 sigbank enables a signal handling pattern where application-level signals are handled
 at specified signal-exchange points and system-level signals are always unblocked

 this allows for blocking system calls to be interrupted by the receipt of system-level
 signals, while maintaining well-defined semantics for two processes to exchange
 application-level signals

*/

#define restrict __restrict

// the signal handler is a single-instruction assignment to this variable
extern volatile int g_signum;

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
//  suspends execution pending receipt of some signal, then returns the received signal
//
// PARAMETERS
//  sig  - (returns) the signal received
//
// REMARKS
//  this api is used to receive application-level signals ; system-level signals can be
//  recived at any time and should be received via g_signum directly
//
// RETURNS
//  xapi semantics
//
int sigreceive(int * const restrict sig)
	__attribute__((nonnull));

#undef restrict
#endif
