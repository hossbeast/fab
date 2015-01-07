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

#ifndef _XLINUX_XSIGNAL_H
#define _XLINUX_XSIGNAL_H

#include <sys/types.h>
#include <signal.h>

/// xkill
//
// SUMMARY
//  proxy for kill
//
// PARAMETERS
//  pid - pid
//  sig - signal
//
int xkill(pid_t pid, int sig);

/// uxkill
//
// SUMMARY
//  proxy for kill that only fails when errno != ESRCH (no such pid)
//
// PARAMETERS
//  pid - pid
//  sig - signal
//  [r] - (returns) the return value from kill
//
int uxkill(pid_t pid, int sig, int * r);

/// xsignal
//
// SUMMARY
//  xapi proxy for signal
//
int xsignal(int signum, sighandler_t handler);

/// xsigaction
//
// SUMMARY
//  xapi proxy for sigaction
//
int xsigaction(int signum, const struct sigaction * act, struct sigaction * oldact)
	__attribute__((nonnull(2)));

#endif
