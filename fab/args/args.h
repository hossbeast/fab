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

#ifndef _ARGS_H
#define _ARGS_H

#include <sys/types.h>

struct selector;

/*
** PER-GN : delete if newest file is older than <policy>  (pertains to a given gn)
** -------------------------------------------------------------------------------------------------------------------------------
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/PRIMARY/stat						stat hash			// rewritten after successfully
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/PRIMARY/content					content hash	// processing a detected change to
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/PRIMARY/vrs							version hash	// the underlying source file
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/PRIMARY/dscv						cached ddisc
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/PRIMARY/afeed_secondary_skipweak/<gn-id>	link to 'gn/<gn-id>' for a SECONDARY gn
**
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/SECONDARY/fab/noforce_ff							// require fabrication of this secondary node due to ff change
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/SECONDARY/fab/noforce_gn							// require fabrication of this secondary node due to primary file change
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/SECONDARY/aneed_primary_skipweak/<gn-id>		link to 'gn/<gn-id>' for a PRIMARY gn
**
** PER-FF : delete if newest file is older than <policy>  (pertains to a given fabfile)
** -------------------------------------------------------------------------------------------------------------------------------
** /var/cache/fab/INIT/<ff-id>/ff/<ff-id>/REGULAR/stat						     	stat hash			// rewritten after successfully
** /var/cache/fab/INIT/<ff-id>/ff/<ff-id>/REGULAR/content					    	content hash	// processing a detected change to
** /var/cache/fab/INIT/<ff-id>/ff/<ff-id>/REGULAR/vrs							    	version hash	// the regular fabfile
** /var/cache/fab/INIT/<ff-id>/ff/<ff-id>/REGULAR/closure_gns/<gn-id>		link to 'gn/<gn-id>' for an enclosed gn
**
** PER-PID : delete if pid is not presently executing     (pertains to a given fab process)
** -------------------------------------------------------------------------------------------------------------------------------
** /var/tmp/fab/pid/<pid>/fml/<fmlvnum>/cmd							cmd text
** /var/tmp/fab/pid/<pid>/fml/<fmlvnum>/stdo						stdout from cmd
** /var/tmp/fab/pid/<pid>/fml/<fmlvnum>/stde						stderr from cmd
*/

#define DEFAULT_INIT_FABFILE 			"./fabfile"
#define DEFAULT_INVALIDATE_ALL		0
#define DEFAULT_MODE_BPLAN				MODE_BPLAN_EXEC
#define DEFAULT_MODE_GNID					MODE_RELATIVE_CWD
#define DEFAULT_MODE_PATHS				MODE_RELATIVE_FABFILE_DIR
#define DEFAULT_MODE_CYCLES				MODE_CYCLES_WARN
#define DEFAULT_CONCURRENCY_LIMIT	0
#define DEFAULT_MODE_BSLIC				MODE_BSLIC_STD

#ifdef DEBUG
# define DEFAULT_MODE_ERRORS			MODE_ERRORS_UNWIND
#else
# define DEFAULT_MODE_ERRORS			MODE_ERRORS_IMMEDIATE
#endif

#define EXPIRATION_POLICY					(60 * 60 * 24 * 7)		/* 7 days */

#define MODE_TABLE(x)																																														\
/* execution modes */																																														\
	_MODE(MODE_BPLAN_GENERATE							, 0x01	, x)		/* (only) generate the buildplan */											\
	_MODE(MODE_BPLAN_BAKE									, 0x02	, x)		/* bake the buildplan */																\
	_MODE(MODE_BPLAN_EXEC									, 0x03	, x)		/* execute the buildplan */															\
/* path handling modes */																																												\
	_MODE(MODE_RELATIVE_FABFILE_DIR				, 0x04	, x)		/* path relative to initial fabfile dir */							\
	_MODE(MODE_RELATIVE_CWD								, 0x05	, x)		/* path relative to cwd */															\
	_MODE(MODE_ABSOLUTE										, 0x06	, x)		/* absolute path */																			\
	_MODE(MODE_CANONICAL									, 0x07	, x)		/* canonical path */																		\
/* cycle handling modes */																																											\
	_MODE(MODE_CYCLES_WARN								, 0x09	, x)		/* warn when a cycle is detected */											\
	_MODE(MODE_CYCLES_FAIL								, 0x0a	, x)		/* fail when a cycle is detected */											\
	_MODE(MODE_CYCLES_DEAL								, 0x0b	, x)		/* deal when a cycle is detected (halt traversal) */		\
/* error reporting modes */																																											\
	_MODE(MODE_ERRORS_UNWIND							, 0x0d	, x)		/* unwind stack when reporting errors */								\
	_MODE(MODE_ERRORS_IMMEDIATE						, 0x0e	, x)		/* report on immediate error condition only */					\
/* bakescript license modes */																																									\
	_MODE(MODE_BSLIC_STD									, 0x10	, x)		/* bakescripts have the standard license  */						\
	_MODE(MODE_BSLIC_FAB									, 0x11	, x)		/* bakescripts have the fab license */									\
/* sanity checking modes */																																											\
	_MODE(MODE_SANITY_DISABLE							, 0x13	, x)		/* disable sanity checks for liblistwise invocations */	\
	_MODE(MODE_SANITY_ENABLE							, 0x14	, x)		/* enable sanity checks for liblistwise invocations */	\

enum {
#define _MODE(a, b, c) a = b,
MODE_TABLE(0)
#undef _MODE
};

#define _MODE(a, b, c) (c) == b ? #a :
#define MODE_STR(x) MODE_TABLE(x) "unknown"

extern struct g_args_t
{
//
// arguments
//

	int									mode_bplan;									// buildplan mode
	int									mode_gnid;									// gn identification mode
	int									mode_cycles;								// cycle handling mode
	int									mode_paths;									// path generation mode
	int									mode_errors;								// error reporting mode
#if DEVEL
	int									mode_bslic;									// bakescript license mode
	int									mode_sanity;								// sanity checking mode
#endif

	int									concurrency;								// concurrently limiting factor

	char *							bakescript_path;						// path to bakescript

	char **							rootvars;										// root scope variable expressions
	int									rootvarsl;
	int									rootvarsa;

	char **							bakevars;										// baked variables
	int									bakevarsl;
	int									bakevarsa;

	char **							invokedirs;									// root directories for locating invocations
	int									invokedirsl;

	struct selector * 	selectors;									// node selectors
	int									selectorsa;
	int									selectorsl;
	int									selectors_arequery;					// whether any selectors target the QUERY list

	int									invalidationsz;							// invalidate all nodes (-B)
	int									invalidationsz_primary;			// invalidate all primary nodes (-Bp)
	int									invalidationsz_secondary;		// invalidate all secondary nodes (-Bs)
} g_args;

/// args_parse
//
// parses command-line options, populating g_args.
//
// Prints the usage statement and terminates the program if options are
// invalid, for example required options are not present, or invalid
// parameters are given to an option
//
// returns zero on failure (malloc failure, for example)
//
int args_parse(int argc, char** argv);

/// args_teardown
//
// free g_args
//
void args_teardown();

#endif
