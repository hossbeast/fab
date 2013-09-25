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

#ifndef _FF_H
#define _FF_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#include "listwise.h"
#include "listwise/generator.h"

#include "coll.h"
#include "hashblock.h"
#include "path.h"
#include "map.h"

#define restrict __restrict

// FF type table
#define FFT_TABLE(x)																												\
	_FFT(FFT_REGULAR				, 0x01	, x)	/* regular fabfile */								\
	_FFT(FFT_DDISC					, 0x02	, x)	/* dependency discovery fabfile */	\
	_FFT(FFT_VAREXPR				, 0x03	, x)	/* variable expression text */			\
	_FFT(FFT_LISTEXPR				, 0x04	, x)	/* list expression text */					\

enum {
#define _FFT(a, b, c) a = b,
	FFT_TABLE(0)
#undef _FFT
};

#define _FFT(a, b, c) (c) == b ? #a "[" #b "]" :
#define FFT_STRING(x) FFT_TABLE(x) "unknown"

struct gn;

/// fabfile
typedef struct ff_file
{
	uint32_t					type;			// fabfile type
	char *						nofile;		// nofile reference (if invoked via nofile syntax)
	path *						path;			// fabfile path
	char *						idstring;	// identifier string, subject to execution parameters

	struct ff_node *	ffn;			// root ff_node parsed from this ff_file

	union
	{
		// variable expression text
		struct {
			int								id;				// unique id
		};

		// dependency discovery fabfile
		struct {
			struct gn *				dscv_gn;	// associated graph node
		};
		
		// regular fabfile
		struct
		{
			hashblock *				hb;
			int								hb_reload;

			int								count;		// number of times processed

			/*
			** the gn closure of an ff_file is all the gns possibly affected if this file changes
			** (dependencies, formulas, dscv formulas, etc)
			*/
			char *						closure_gns_dir;
			struct gn ** 			closure_gns;
			int								closure_gnsl;
			int								closure_gnsa;

			/*
			** the var closure of an ff_file is all of the variables whose values affect the meaning
			** of the ff_file (FFN_VARREF nodes)
			*/
			struct ff_node **	closure_vars;
			int								closure_varsl;
			int								closure_varsa;
		};
	};
} ff_file;

// collection of ff_files (REGULAR as well as DDISC)
extern union ff_files_t
{
	coll_doubly c;

	struct
	{
		int		l;						// length
		int		a;						// allocated
		int		z;						// element size

		ff_file ** e;				// elements

		map *	by_canpath;
	};
} ff_files;

// ff parser; opaque type
struct ff_parser_t;
typedef struct ff_parser_t ff_parser;

struct ff_parser_t
{
	void *							p;
	generator_parser *	gp;
};

/// ff_mkparser
//
// creates an instance of a ff parser which may be passed repeatedly to parse
//
int ff_mkparser(ff_parser ** const restrict p)
	__attribute__((nonnull));

/// ff_reg_load
//
// SUMMARY
//  load (parsing if necessary) the fabfile at the specified preconstructed path
//  path is copied from ; it should be freed by the caller
//
// PARAMETERS
//  p      - parser returned from mkparser
//  path   - path to fabfile
//  nofile - (only important the first time) whether the fabfile was invoked via nofile syntax
//  ff     - results go here
//
// RETURNS
//  0 on error - check *ff to see if the parse was successful
//
int ff_reg_load(
	  const ff_parser * const restrict p
	, const path * const restrict path
	, char * const restrict nofile
	, const int nofilel
	, ff_file ** const restrict ff
)
	__attribute__((nonnull(1, 2, 5)));

/// ff_dsc_parse
//
// SUMMARY
//  parse text from a dependency discovery fabfile
//
// PARAMETERS
//  p      - parser returned from mkparser
//  fp     - canonical path to the fabfile
//  b      - buffer to parse from
//  sz     - number of bytes to parse
//  dscvgn - gn for discovery 
//  ff     - results go here
//
// RETURNS
//  0 on error - check *ff to see if the parse was successful
//
int ff_dsc_parse(
	  const ff_parser * const restrict p
	, char * b
	, int sz
	, const char * const restrict fp 
	, struct gn * dscv_gn
	, ff_file ** const restrict ff
)
	__attribute__((nonnull));

/// ff_var_parse
//
// SUMMARY
//  parse variable expressions
//
// PARAMETERS
//  p      - parser returned from mkparser
//  b      - buffer to parse from
//  sz     - number of bytes to parse
//  id     - unique id for this variable expression text
//  ff     - results go here
//
int ff_var_parse(
	  const ff_parser * const restrict p
	, char * b
	, int sz
	, int id
	, ff_file ** const restrict ff
)
	__attribute__((nonnull));

/// ff_list_parse
//
// SUMMARY
//  parse list expressions
//
// PARAMETERS
//  p      - parser returned from mkparser
//  b      - buffer to parse from
//  sz     - number of bytes to parse
//  id     - unique id for this list expression text
//  ff     - results go here
//
int ff_list_parse(
	  const ff_parser * const restrict p
	, char * b
	, int sz
	, int id
	, ff_file ** const restrict ff
)
	__attribute__((nonnull));

/// ff_freeparser
//
// free a ff parser returned from mkparser with free semantics
//
void ff_freeparser(ff_parser* const restrict);

/// ff_xfreeparser
//
// free a ff parser returned from mkparser with xfree semantics
//
void ff_xfreeparser(ff_parser ** const restrict)
	__attribute__((nonnull));

/// ff_idstring
//
// get a string identifying a ff, subject to execution arguments
//
char * ff_idstring(ff_file * const restrict ff)
	__attribute__((nonnull));

/// ff_regular_reload
//
// for a REGULAR ff_file - load the previous hashblock, stat the file
//
int ff_regular_reload(ff_file * const restrict ff)
	__attribute__((nonnull));

/// ff_regular_reconcile
//
// for all REGULAR ff_files
//  - 
//
//  - write the current hashblock
//
int ff_regular_reconcile();

/// ff_teardown
//
// free ff_files
//
void ff_teardown();

/// ff_regular_enclose_gn
//
// SUMMARY
//  mark a regular ff_file as affecting a gn by appending gn to its closure_gns list
//
// PARAMETERS
//
int ff_regular_enclose_gn(ff_file * const restrict ff, struct gn * const restrict gn)
	__attribute__((nonnull));

/// ff_dump
//
// SUMMARY
//  log an ff_file under FFFILE
//
void ff_dump(ff_file * const restrict ff)
	__attribute__((nonnull));

#undef restrict
#endif
