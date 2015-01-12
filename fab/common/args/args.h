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

#ifndef _FAB_ARGS_H
#define _FAB_ARGS_H

#include <stdint.h>
#include <sys/types.h>

struct memblk;			// common/memblk.h
struct selector;		// selector.h
struct path;				// path.h

/*
** PER-PID : delete if pid is not presently executing     (pertains to a given fab process)
** -------------------------------------------------------------------------------------------------------------------------------
** /var/tmp/fab/pid/<pid>/fml/<fmlvnum>/cmd							cmd text
** /var/tmp/fab/pid/<pid>/fml/<fmlvnum>/stdo						stdout from cmd
** /var/tmp/fab/pid/<pid>/fml/<fmlvnum>/stde						stderr from cmd
*/

#define SELECTOR_TABLE(x)																												\
	_SELECTOR(SELECTOR_FABRICATE		, 0x01	, x)	/* fabricate */									\
	_SELECTOR(SELECTOR_FABRICATEX		, 0x02	, x)	/* fabricate-exact */						\
	_SELECTOR(SELECTOR_FABRICATEN		, 0x04	, x)	/* fabricate-nofile */					\
	_SELECTOR(SELECTOR_INVALIDATE		, 0x08	, x)	/* invalidate */								\
	_SELECTOR(SELECTOR_DISCOVERY		, 0x10	, x)	/* discovery */									\
	_SELECTOR(SELECTOR_INSPECT			, 0x20	, x)	/* inspect */										\
	_SELECTOR(SELECTOR_QUERY				, 0x40	, x)	/* query */

enum {
#define _SELECTOR(a, b, c) a = b,
SELECTOR_TABLE(0)
#undef _SELECTOR
};

#define _SELECTOR(a, b, c) (c) == b ? #a :
#define SELECTOR_STR(x) SELECTOR_TABLE(x) "unknown"

#define SELECTOR_BASE_TABLE(x)																																		\
	_SELECTOR_BASE(SELECTOR_BASE_FABFILE_DIR	, 0x01	, x)	/* resolve against init-fabfile-dir */	\
	_SELECTOR_BASE(SELECTOR_BASE_CWD					, 0x02	, x)	/* resolve against cwd */								\

enum {
#define _SELECTOR_BASE(a, b, c) a = b,
SELECTOR_BASE_TABLE(0)
#undef _SELECTOR_BASE
};

#define _SELECTOR_BASE(a, b, c) (c) == b ? #a :
#define SELECTOR_BASE_STR(x) SELECTOR_BASE_TABLE(x) "unknown"

// node selectors
typedef struct selector {
	char * 			s;					// selector text
	int					sl;					// selector text length
	char				mode;				// '-' : remove, '+' : append
	uint8_t			base;				// base for path resolution
	uint32_t		lists;			// list(s) affected
} selector;

#define DEFAULT_INIT_FABFILE 			"./fabfile"
#define DEFAULT_INVALIDATE_ALL		0
#define DEFAULT_MODE_BPLAN				MODE_BPLAN_EXEC
#define DEFAULT_MODE_GNID					MODE_RELATIVE_CWD
#define DEFAULT_MODE_PATHS				MODE_RELATIVE_FABFILE_DIR
#define DEFAULT_MODE_CYCLES				MODE_CYCLES_WARN
#define DEFAULT_CONCURRENCY_LIMIT	-1

#if DEBUG || DEVEL
# define DEFAULT_MODE_BACKTRACE		MODE_BACKTRACE_FULL
# define DEFAULT_MODE_LOGTRACE		MODE_LOGTRACE_NONE
#endif
#if DEVEL
# define DEFAULT_MODE_BSLIC				MODE_BSLIC_STD
#endif

#if SANITY
# define DEFAULT_MODE_SANITY			MODE_SANITY_DISABLE
#endif

#define MODE_TABLE(x)																																														\
/* execution modes */																																														\
	_MODE(MODE_BPLAN_GENERATE							, 0x01	, x)		/* (only) generate the buildplan */											\
	_MODE(MODE_BPLAN_BUILDSCRIPT					, 0x02	, x)		/* create a buildscript from the buildplan */						\
	_MODE(MODE_BPLAN_EXEC									, 0x03	, x)		/* execute the buildplan */															\
/* path handling modes */																																												\
	_MODE(MODE_RELATIVE_FABFILE_DIR				, 0x04	, x)		/* path relative to initial fabfile dir */							\
	_MODE(MODE_RELATIVE_CWD								, 0x05	, x)		/* path relative to cwd */															\
	_MODE(MODE_ABSOLUTE										, 0x06	, x)		/* absolute path */																			\
	_MODE(MODE_CANONICAL									, 0x07	, x)		/* canonical path */																		\
/* cycle handling modes */																																											\
	_MODE(MODE_CYCLES_WARN								, 0x08	, x)		/* warn when a cycle is detected */											\
	_MODE(MODE_CYCLES_FAIL								, 0x09	, x)		/* fail when a cycle is detected */											\
	_MODE(MODE_CYCLES_DEAL								, 0x0a	, x)		/* deal when a cycle is detected (halt traversal) */
#if DEBUG || DEVEL
#define MODE_TABLE_DEBUG(x)																																											\
/* error reporting modes */																																											\
	_MODE(MODE_BACKTRACE_FULL							, 0x0b	, x)		/* report on immediate error condition only */					\
	_MODE(MODE_BACKTRACE_PITHY	 					, 0x0c	, x)		/* unwind stack when reporting errors */								\
	_MODE(MODE_LOGTRACE_NONE							, 0x0d	, x)		/* disable log trace */																	\
	_MODE(MODE_LOGTRACE_FULL							, 0x0e	, x)		/* enable log trace */
#endif
#if DEVEL
#define MODE_TABLE_DEVEL(x)																																											\
/* buildscript license modes */																																									\
	_MODE(MODE_BSLIC_STD									, 0x10	, x)		/* buildscripts have the standard license  */						\
	_MODE(MODE_BSLIC_FAB									, 0x11	, x)		/* buildscripts have the fab license */
#endif

enum {
#define _MODE(a, b, c) a = b,
#if DEBUG || DEVEL
MODE_TABLE_DEBUG(0)
#endif
#if DEVEL
MODE_TABLE_DEVEL(0)
#endif
MODE_TABLE(0)
#undef _MODE
};

#define _MODE(a, b, c) (c) == b ? #a :
#if DEBUG && DEVEL
# define MODE_STR(x) MODE_TABLE(x) MODE_TABLE_DEBUG(x) MODE_TABLE_DEVEL(x) "UNKNWN"
#elif DEBUG
# define MODE_STR(x) MODE_TABLE(x) MODE_TABLE_DEBUG(x) "UNKNWN"
#elif DEVEL
# define MODE_STR(x) MODE_TABLE(x) MODE_TABLE_DEVEL(x) "UNKNWN"
#else
# define MODE_STR(x) MODE_TABLE(x) "UNKNWN"
#endif

extern struct g_args_t
{
//
// arguments
//
	char *							argvs;											// copy of logger::g_argvs

	int									mode_bplan;									// buildplan mode
	int									mode_gnid;									// gn identification mode
	int									mode_cycles;								// cycle handling mode
	int									mode_paths;									// path generation mode
#if DEBUG || DEVEL
	int									mode_backtrace;							// backtrace reporting mode
	int									mode_logtrace;							// log trace mode
#endif
#if DEVEL
	int									mode_bslic;									// buildscript license mode
#endif

	int									concurrency;								// concurrently limiting factor

	char *							buildscript_path;						// path to buildscript

	char **							rootvars;										// root scope variable expressions
	int									rootvarsl;
	int									rootvarsa;

	char **							bs_runtime_vars;						// buildscript variables that are settable at runtime
	int									bs_runtime_varsl;
	int									bs_runtime_varsa;

	char **							invokedirs;									// root directories for locating invocations
	int									invokedirsl;

	struct selector * 	selectors;									// node selectors
	int									selectorsa;
	int									selectorsl;
	int									selectors_arequery;					// whether any selectors target the QUERY list
	int									selectors_areinspections;		// whether any selectors target the INSPECT list
	int									selectors_arediscovery;			// whether any selectors target the DISCOVERY list

	int									invalidationsz;							// invalidate all nodes (-B)
	int									invalidationsz_primary;			// invalidate all primary nodes (-Bp)
	int									invalidationsz_secondary;		// invalidate all secondary nodes (-Bs)

	struct path *				init_fabfile_path;					// path to initial fabfile
} * g_args;

#define restrict __restrict

/// args_parse
//
// SUMMARY
//  parses command-line options, populating g_args.
//
// REMARKS
//  prints the usage statement and terminates the program if options are
//  invalid, for example required options are not present, or invalid
//  parameters are given to an option
//
int args_parse();

/// args_summarize
//
// SUMMARY
//  use logger to output a summary of params/args as received
//
int args_summarize();

/// args_teardown
//
// free g_args
//
void args_teardown();

/// args_freeze
//
// SUMMARY
//  
//
void args_freeze(struct memblk * const restrict mb)
	__attribute__((nonnull));

void args_thaw(char * const restrict p)
	__attribute__((nonnull));

#undef restrict
#endif
