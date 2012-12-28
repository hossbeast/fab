#ifndef _FF_H
#define _FF_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#include <listwise.h>
#include <listwise/generator.h>

#include "coll.h"
#include "hashblock.h"

#define restrict __restrict

// flags
#define FFN_SINGLE		0x01
#define FFN_MULTI			0x02
#define FFN_DISCOVERY	0x04
#define FFN_WEAK			0x08

// FF node type table
#define FFN_TABLE(x)										\
	_FFN(FFN_STMTLIST				, 0x01	, x)	\
	_FFN(FFN_DEPENDENCY			, 0x02	, x)	\
	_FFN(FFN_FORMULA				, 0x04	, x)	\
	_FFN(FFN_INCLUDE				, 0x05	, x)	\
	_FFN(FFN_VARDECL				, 0x06	, x)	\
	_FFN(FFN_LIST						, 0x07	, x)	\
	_FFN(FFN_GENERATOR			, 0x08	, x)	\
	_FFN(FFN_VARNAME				, 0x09	, x)	\
	_FFN(FFN_LF							, 0x0a	, x)	\
	_FFN(FFN_WORD						, 0x0b	, x)

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
	FFT_TABLE
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

	char *						name;			// name of fabfile
	char *						path;			// canonical path to fabfile 
	char *						dir;			// canonical path to dir fabfile is in
	char *						idstring;	// identifier string, subject to execution parameters

	union
	{
		// dependency discovery fabfile
		struct {
			struct gn *				dscv_gn;	// associated graph node
		};
		
		// regular fabfile
		struct {
			hashblock *				hb;
		};
	};
} ff_file;

// collection of ff_files (REGULAR only, not discovery)
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
} ff_loc;

typedef struct ff_node
{
	uint32_t		type;		// node type
	ff_loc			loc;		// location of the node

	char*				s;			// string value of the entire node
	int					l;			// string length

	generator * 		generator;		// FFN_GENERATOR
	uint8_t					flags;				// FFN_DEPENDENCY, FFN_FORMULA

	union {
		char*	strings[1];

		struct {													// FFN_WORD, FFN_GENERATOR
			char*			text;
		};

		struct {													// FFN_VARNAME, FFN_VARDECL
			char*			name;
		};
	};

	union {
		struct ff_node*			nodes[2];

		struct {													// FFN_LIST
			struct ff_node*			generator_node;
		};

		struct {													// FFN_VARDECL
			struct ff_node*			definition;
		};

		struct {													// FFN_FORMULA
			struct ff_node*			targets;
		};

		struct {													// FFN_DEPENDENCY
			struct ff_node*			needs;
			struct ff_node*			feeds;
		};
	};

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
	};

	// implementation
	char*							e;
	struct ff_node*		chain[1];		// chains for this node
	struct ff_node*		next;				// next sibling in parent chain
} ff_node;

typedef struct
{
	ff_node ** const		ffn;				// results go here
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

	ff_file *						ff;					// info about the fabfile

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
#define ff_mkparser_onfail "p: %p"
int ff_mkparser(ff_parser ** const restrict p)
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
#define ff_parse_onfail "p: %p, path: '%s', ff: %p"
int ff_parse(
	  const ff_parser * const restrict p
	, char * path
	, ff_node ** const restrict ffn
)
	__attribute__((nonnull));

/// ff_dsc_parse
//
// see ff_parse, but for discovery files, which support only a limited syntax
//
#define ff_dsc_parse_onfail "p: %p, path: '%s', ff: %p"
int ff_dsc_parse(
	  const ff_parser * const restrict p
	, char * text
	, int l
	, char * path
	, struct gn * dscv_gn
	, ff_node ** const restrict ffn
);

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


int ff_hb_read(ff_file * const restrict ff)
	__attribute__((nonnull));
	

int ff_hb_write(ff_file * const restrict ff)
	__attribute__((nonnull));

#undef restrict
#endif
