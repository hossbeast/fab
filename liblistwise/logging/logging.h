/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LISTWISE_LOGGING_H
#define _LISTWISE_LOGGING_H

#include "listwise.h"

#define restrict __restrict

#if DEBUG || DEVEL || SANITY
struct listwise_logging
{
#if DEBUG
	// generator_log
	void (*log_generator)(void * udata, const char * func, const char * file, int line, char * fmt, ...);

	// lstack_log
	void (*log_lstack)(void * udata, const char * func, const char * file, int line, char * fmt, ...);

	// exec progress - also invokes log_lstack to dump lstack along the way
	void (*log_exec)(void * udata, const char * func, const char * file, int line, char * fmt, ...);

	// listwise operators - info
	void (*log_opinfo)(void * udata, const char * func, const char * file, int line, char * fmt, ...);
#endif

#if DEVEL
	// generator parsing - tokens
	void (*log_tokens)(void * udata, const char * func, const char * file, int line, char * fmt, ...);

	// generator parsing - states
	void (*log_states)(void * udata, const char * func, const char * file, int line, char * fmt, ...);
#endif

#if SANITY
	// listwise sanity checks
	void (*log_sanity)(void * udata, const char * func, const char * file, int line, char * fmt, ...);
#endif
} * listwise_logging_config;

/// listwise_configure_logging
//
// SUMMARY
//  configure liblistwise logging mechanism
//
void listwise_logging_configure(struct listwise_logging *)
	__attribute__((nonnull));
#endif

#undef restrict
#endif
