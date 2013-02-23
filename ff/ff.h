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

#define restrict __restrict

// flags
#define FFN_SINGLE			0x01
#define FFN_MULTI				0x02
#define FFN_DISCOVERY		0x04
#define FFN_FABRICATION	0x08
#define FFN_WEAK				0x10
#define FFN_GATED				0x20

// FF node type table
#define FFN_TABLE(x)										\
	_FFN(FFN_STMTLIST				, 0x01	, x)	\
	_FFN(FFN_DEPENDENCY			, 0x02	, x)	\
	_FFN(FFN_FORMULA				, 0x03	, x)	\
	_FFN(FFN_INVOCATION			, 0x04	, x)	\
	_FFN(FFN_VARASSIGN			, 0x05	, x)	\
	_FFN(FFN_VARPUSH				, 0x06	, x)	\
	_FFN(FFN_VARPOP					, 0x07	, x)	\
	_FFN(FFN_VARDESIGNATE		, 0x08	, x)	\
	_FFN(FFN_LIST						, 0x09	, x)	\
	_FFN(FFN_GENERATOR			, 0x0a	, x)	\
	_FFN(FFN_VARREF					, 0x0b	, x)	\
	_FFN(FFN_LF							, 0x0c	, x)	\
	_FFN(FFN_WORD						, 0x0d	, x)

enum {
#define _FFN(a, b, c) a = b,
	FFN_TABLE(0)
#undef _FFN
};

#define _FFN(a, b, c) (c) == b ? #a "[" #b "]" :
#define FFN_STRING(x) FFN_TABLE(x) "unknown"

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

struct ff_file;
struct ff_loc;
struct ff_node;
struct gn;

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

typedef struct ff_loc
{
	int				f_lin;
	int				f_col;
	int				l_lin;
	int				l_col;

	ff_file *	ff;
} __attribute__((packed)) ff_loc;

typedef struct ff_node
{
	uint32_t		type;		// node type
	ff_loc			loc;		// location of the node

	char*				s;			// string value of the entire node
	int					l;			// string length

	generator * 		generator;		// FFN_GENERATOR
	uint8_t					flags;				// FFN_DEPENDENCY, FFN_FORMULA, FFN_DESIGNATE

	/*
	** strings are freed in freenode
	*/
	union {
		char*	strings[1];

		struct {													// FFN_WORD, FFN_GENERATOR
			char*			text;
		};

		struct {													// FFN_VARREF, FFN_VARASSIGN, FFN_VARPUSH, FFN_VARPOP
			char*			name;
		};
	};

	/*
	** nodes_owned are freed in freenode
	*/
	union {
		struct ff_node*			nodes_owned[2];

		struct {													// FFN_LIST
			struct ff_node*			generator_node;
		};

		struct {													// FFN_VARASSIGN, FFN_VARPUSH, FFN_VARPOP, FFN_DESIGNATE
			struct ff_node*			definition;
		};

		struct {													// FFN_FORMULA
			struct ff_node*			targets_0;
		};

		struct {													// FFN_DEPENDENCY
			struct ff_node*			needs;
			struct ff_node*			feeds;
		};

		struct {													// FFN_INVOCATION
			struct ff_node*			modules;
			struct ff_node*			scope;
		};
	};

	/*
	** nodes_notowned are not freed
	*/
	union {
		struct ff_node*			nodes_notowned[2];

		struct {													// FFN_FORMULA
			struct ff_node*			targets_1;
		};
	};

	/*
	** chain[0] is accumulated and converted to list in flatten
	** list is freed in freenode
	*/
	union {
		struct {
			struct ff_node**	list;
			int								list_l;
			int								list_a;
		};

		struct {											// FFN_STMTLIST
			struct ff_node**	statements;
			int								statements_l;
		};

		struct {											// FFN_LIST
			struct ff_node**	elements;
			int								elements_l;
		};

		struct {											// FFN_FORMULA
			struct ff_node**	commands;
			int								commands_l;
		};

		struct {											// FFN_INVOCATION
			struct ff_node**	designations;
			int								designations_l;
		};

		struct {											// FFN_DESIGNATE
			struct ff_node**	vars;
			int								vars_l;
		};
	};

	// implementation
	char*							e;
	struct ff_node*		chain[1];		// chains for this node
	struct ff_node*		next;				// next sibling in parents chain
} ff_node;

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
// parse a fabfile using a preconstructed path
//
// parameters
//
//   p    - parser returned from mkparser
//   path - path to fabfile
//   ff   - results go here
//
// returns
//  0 on error - check *ff to see if the parse was successful
//
int ff_parse_path(
	  const ff_parser * const restrict p
	, const path * const restrict path
	, ff_node ** const restrict ffn
)
	__attribute__((nonnull));

/// ff_parse
//
// parse a fabfile
//
// parameters
//
//   p    - parser returned from mkparser
//   path - path to fabfile
//   ff   - results go here
//
// returns
//  0 on error - check *ff to see if the parse was successful
//
int ff_parse(
	  const ff_parser * const restrict p
	, const char * const restrict fp
	, const char * const restrict base 
	, ff_node ** const restrict ffn
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
	, ff_node ** const restrict ffn
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

/// ff_freenode
//
// free a ff_node with free semantics
//
void ff_freenode(ff_node * const restrict);

/// ff_xfreenode
//
// free a ff_node with xfree semantics
//
void ff_xfreenode(ff_node ** const restrict)
	__attribute__((nonnull));

/// ff_dump
//
//
//
void ff_dump(ff_node * const restrict root);

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
int ff_regular_affecting_gn(struct ff_file * const restrict ff, struct gn * const restrict gn)
	__attribute__((nonnull));

#undef restrict
#endif
