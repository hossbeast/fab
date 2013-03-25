#ifndef _FF_H
#define _FF_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#include <listwise.h>
#include <listwise/generator.h>

#include "coll.h"
#include "hashblock.h"
#include "path.h"
#include "map.h"

#define restrict __restrict

// FF type table
#define FFT_TABLE(x)																												\
	_FFT(FFT_REGULAR				, 0x01	, x)	/* regular fabfile */								\
	_FFT(FFT_DDISC					, 0x02	, x)	/* dependency discovery fabfile */	\

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
	path *						path;			// fabfile path
	char *						idstring;	// identifier string, subject to execution parameters

	struct ff_node *	ffn;			// root ff_node parsed from this ff_file

	union
	{
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

/// ff_parse_path
//
// SUMMARY
//  load (parsing if necessary) the fabfile at the specified preconstructed path
//  path is copied from ; it should be freed by the caller
//
// PARAMETERS
//  p    - parser returned from mkparser
//  path - path to fabfile
//  ff   - results go here
//
// RETURNS
//  0 on error - check *ff to see if the parse was successful
//
int ff_reg_load(
	  const ff_parser * const restrict p
	, const path * const restrict path
	, ff_file ** const restrict ff
)
	__attribute__((nonnull));

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

/// ff_regular_rewrite
//
// for a REGULAR ff_file - write the current hashblock
//
int ff_regular_rewrite(ff_file * const restrict ff)
	__attribute__((nonnull));

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
