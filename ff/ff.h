#ifndef _FF_H
#define _FF_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#define restrict __restrict

#define FFN_TABLE(x)										\
	_FFN(FFN_DEPENDENCY			, 0x01	, x)	\
	_FFN(FFN_FNAME					, 0x02	, x)	\
	_FFN(FFN_FORMULA				, 0x03	, x)	\
	_FFN(FFN_COMMAND_TEXT		, 0x04	, x)	\
	_FFN(FFN_COMMAND_REF		, 0x05	, x)	\

enum {
#define _FFN(a, b, c) a = b,
	FFN_TABLE(0)
#undef _FFN
};

#define _FFN(a, b, c) (c) == b ? #a "[" #b "]" :
#define FFN_STRING(x) FFN_TABLE(x) "unknown"

struct ff_loc;
typedef struct ff_loc
{
	int f_lin;
	int f_col;
	int l_lin;
	int l_col;
} ff_loc;

struct ff_node;
typedef struct ff_node
{
	uint32_t		type;
	ff_loc			loc;

	// normalized directory that the fabfile is in
	char*				ff_dir;	

	char*				s;
	int					l;

	union {
		char*	strings[1];

		struct {											// FFN_FNAME
			char*			name;
		};

		struct {											// FFN_COMMAND_TEXT
			char*			text;
		};
	};

	union {
		int numbers[1];

		struct {											// FFN_COMMAND_REF
			int				ref;
		};
	};

	union {
		struct {
			struct ff_node**	list_one;
			int								list_one_l;
			int								list_one_a;
		};

		struct {											// FFN_FORMULA, FFN_DEPENDENCY
			struct ff_node**	targets;
			int								targets_l;
		};
	};

	union {
		struct {
			struct ff_node**	list_two;
			int								list_two_l;
			int								list_two_a;
		};

		struct {											// FFN_FORMULA
			struct ff_node**	commands;
			int								commands_l;
		};

		struct {											// FFN_DEPENDENCY
			struct ff_node**	prereqs;
			int								prereqs_l;
		};
	};

	// implementation
	char*							e;
	struct ff_node*		chain[2];		// chains for this node
	struct ff_node*		next;				// next sibling in parent chain
} ff_node;

typedef struct
{
	ff_node ** const		ffn;				// results go here
	void*								scanner;		// scanner

	const char*					orig_base;	// ptr to original input string
	int									orig_len;		// length of original input string
	char*								act_base;		// ptr to lexer copy

	char*								ff_dir;			// relative path to the directory the fabfile is in

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
	, char* path
	, ff_node ** const restrict ff
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

#endif
