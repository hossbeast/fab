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

typedef int (*listwise_logging_would)(void * token, void * udata);
typedef void (*listwise_logging_log)(void* token, void * udata, const char * func, const char * file, int line, char * fmt, ...);

struct listwise_logging
{
	// generator_log
	void * 									generator_token;
	listwise_logging_would	generator_would;
	listwise_logging_log		generator_log;

	// lstack_log
	void * 									lstack_token;
	listwise_logging_would	lstack_would;
	listwise_logging_log		lstack_log;

	// exec progress - also invokes lstack_log to dump lstack along the way
	void * 									exec_token;
	listwise_logging_would	exec_would;
	listwise_logging_log		exec_log;

	// listwise operators - info
	void * 									opinfo_token;
	listwise_logging_would	opinfo_would;
	listwise_logging_log		opinfo_log;

#if DEVEL
	// generator parsing - tokens
	void * 									tokens_token;
	listwise_logging_would	tokens_would;
	listwise_logging_log		tokens_log;

	// generator parsing - states
	void * 									states_token;
	listwise_logging_would	states_would;
	listwise_logging_log		states_log;

	// listwise sanity checks
	void * 									sanity_token;
	listwise_logging_would	sanity_would;
	listwise_logging_log		sanity_log;
#endif
} * listwise_logging_config;

/// listwise_configure_logging
//
// SUMMARY
//  configure liblistwise logging mechanism
//
void listwise_logging_configure(struct listwise_logging *)
	__attribute__((nonnull));

#undef restrict
#endif
