#ifndef _FF_H
#define _FF_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#include <listwise.h>
#include <listwise/generator.h>

#include "ffn.h"

#include "coll.h"
#include "hashblock.h"
#include "path.h"

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
		struct {
			struct gn ** 			affected_gn;	// all gn's related to this ff_file (dependencies, formulas, dscv formulas, etc)
			int								affected_gnl;
			int								affected_gna;

			char *						affected_dir;

			hashblock *				hb;
			int								hb_reload;

			struct ff_node **	affecting_vars;	// flat list of VARREF's in this fabfile
			int								affecting_varsl;
			int								affecting_varsa;
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
	};
} ff_files;

typedef struct
{
	ff_file *						ff;					// fabfile being parsed; ffn is at ff->ffn
	void*								scanner;		// scanner

	int									states[64];	// start states stack
	int									states_n;

	ff_loc							loc;				// running location track for this parse
	int									badchar;		// encountered unknown character

	ff_loc							last_loc;
	int									last_tok;
	const char*					last_s;
	const char*					last_e;

	const char*					orig_base;	// ptr to original input string
	int									orig_len;		// length of original input string
	char*								act_base;		// ptr to lexer copy

	int									r;					// zeroed in yyerror
} parse_param;

void ff_yyerror(void* loc, void* scanner, parse_param* pp, char const* err);

// ff parser; opaque type
struct ff_parser_t;
typedef struct ff_parser_t ff_parser;

/// ff_mkparser
//
// creates an instance of a ff parser which may be passed repeatedly to parse
//
int ff_mkparser(ff_parser ** const restrict p)
	__attribute__((nonnull));

/// ff_parse_path
//
// SUMMARY
//  parse a fabfile using a preconstructed path
//
// PARAMETERS
//  p    - parser returned from mkparser
//  path - path to fabfile
//  ff   - results go here
//
// RETURNS
//  0 on error - check *ff to see if the parse was successful
//
int ff_parse_path(
	  const ff_parser * const restrict p
	, const path * const restrict path
	, ff_file ** const restrict ff
)
	__attribute__((nonnull));

/// ff_parse
//
// SUMMARY
//  parse a fabfile
//
// PARAMETERS
//  p    - parser returned from mkparser
//  path - path to fabfile
//  ff   - results go here
//
// RETURNS
//  0 on error - check *ff to see if the parse was successful
//
int ff_parse(
	  const ff_parser * const restrict p
	, const char * const restrict fp
	, const char * const restrict base 
	, ff_file ** const restrict ff
)
	__attribute__((nonnull));

/// ff_dsc_parse
//
// see ff_parse, but for discovery files, which support only a limited syntax
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

/// ff_regular_affecting_gn
//
// SUMMARY
//  mark a regular ff_file as affecting a gn by appending gn to its affecting_gn list
//
// PARAMETERS
//
int ff_regular_affecting_gn(ff_file * const restrict ff, struct gn * const restrict gn)
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
