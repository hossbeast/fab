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

#ifndef _TS_H
#define _TS_H

#include <stdint.h>
#include <sys/types.h>

#include "ff.h"
#include "ffn.h"

#include "pstring.h"
#include "map.h"

struct fml;			// fml.h
struct ff_loc;	// ffn.h

typedef struct fmlctx
{
	struct fml *			fml;		// the formula
	map *							bag;		// bag of values evaluated at parse-time

	struct ff_loc  ** locs;		// [0] is the top of the loc stack
	int								locsl;
} fmlctx;

/// fmleval
//
// an instance of a formula evaluation with a list of products
//
typedef struct fmleval
{
	fmlctx * 			ctx;					// eval context
	uint32_t			flags;

	union
	{
		struct 										// FFN_FABRICATION
		{
			struct gn **	products;	// nodes updated by executing the fmlv
			int						productsl;
		};

		struct										// FFN_DISCOVERY
		{
			struct gn *		target;		// primary node whose source file provides discovery info

			// tracking
			int						dscv_mark;
		};
	};
} fmleval;

// thread workspace
typedef struct 
{
	ff_parser *				ffp;					// ff parser
	ff_node *					ffn;					// ff node

	struct fmleval *	fmlv;					// fml evaluation context

	pid_t							pid;					// child pid

	int								cmd_fd;				// fd for cmd file
	pstring *					cmd_path;			// full path to cmd file
	pstring *					cmd_txt;			// txt in cmd file

	int								stdo_fd;			// fd for stdout file
	pstring *					stdo_path;		// full path to stdout file
	pstring *					stdo_txt;			// txt in stdout file

	int								stde_fd;			// fd for stderr file
	pstring *					stde_path;		// full path to stderr file
	pstring *					stde_txt;			// txt in stderr file

	int								r_status;			// exit status
	int								r_signal;			// exit signal

	int y;
} ts;

/// ts_ensure
//
// SUMMARY
//  ensure that at least n threadspaces are allocated
//  then call ts_reset on the first n threadspaces
//
int ts_ensure(ts *** ts, int * tsa, int n)
	__attribute__((nonnull));

/// ts_reset
//
// reset all threadspace properties without freeing underlying resources
// so that it can be used again
//
void ts_reset(ts * ts)
	__attribute__((nonnull));

/// ts_execwave
//
// SUMMARY
//  execute a set of formulas each associated with a threadspace
//
// PARAMETERS
//  ts    - ptr to threadspaces
//  n     - number of threadspaces
//  id    - process-unique identifier for this wave of threadspace executions
//  no    - wave number for logging (buildplan stage, etc)
//  hi    - log tags to associate with high-level logging of this execution wave
//  lo    - log tags to associate with low-level logging of this execution wave
//  [bad] - set to the number of formulas that failed
//
// RETURNS
//  nonzero on success - (some) formulas may have failed, check *bad
//
int ts_execwave(ts ** ts, int n, int * waveid, int waveno, uint64_t hi, int * bad)
	__attribute__((nonnull(1, 3)));

/// ts_free
//
// free a threadspace with free semantics
//
void ts_free(ts * ts);

#endif

