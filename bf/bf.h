#ifndef _BF_H
#define _BF_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#define restrict __restrict

#define BFN_TABLE(x)										\
	_BFN(BFN_DEPENDENCY			, 0x01	, x)	\
	_BFN(BFN_FNAME					, 0x02	, x)	\
	_BFN(BFN_FORMULA				, 0x03	, x)	\
	_BFN(BFN_COMMAND_TEXT		, 0x04	, x)	\
	_BFN(BFN_COMMAND_REF		, 0x05	, x)	\

enum {
#define _BFN(a, b, c) a = b,
	BFN_TABLE(0)
#undef _BFN
};

#define _BFN(a, b, c) (c) == b ? #a "[" #b "]" :
#define BFN_STRING(x) BFN_TABLE(x) "unknown"

struct bf_loc;
typedef struct bf_loc
{
	int f_lin;
	int f_col;
	int l_lin;
	int l_col;
} bf_loc;

struct bf_node;
typedef struct bf_node
{
	uint32_t		type;
	bf_loc			loc;

	// normalized directory that the buildfile is in
	char*				bf_dir;	

	char*				s;
	int					l;

	union {
		char*	strings[1];

		struct {											// BFN_FNAME
			char*			name;
		};

		struct {											// BFN_COMMAND_TEXT
			char*			text;
		};
	};

	union {
		int numbers[1];

		struct {											// BFN_COMMAND_REF
			int				ref;
		};
	};

	union {
		struct {
			struct bf_node**	list_one;
			int								list_one_l;
			int								list_one_a;
		};

		struct {											// BFN_FORMULA, BFN_DEPENDENCY
			struct bf_node**	targets;
			int								targets_l;
		};
	};

	union {
		struct {
			struct bf_node**	list_two;
			int								list_two_l;
			int								list_two_a;
		};

		struct {											// BFN_FORMULA
			struct bf_node**	commands;
			int								commands_l;
		};

		struct {											// BFN_DEPENDENCY
			struct bf_node**	prereqs;
			int								prereqs_l;
		};
	};

	// implementation
	char*							e;
	struct bf_node*		chain[2];		// chains for this node
	struct bf_node*		next;				// next sibling in parent chain
} bf_node;

typedef struct
{
	bf_node ** const		bfn;				// results go here
	void*								scanner;		// scanner

	const char*					orig_base;	// ptr to original input string
	int									orig_len;		// length of original input string
	char*								act_base;		// ptr to lexer copy

	char*								bf_dir;			// relative path to the directory the buildfile is in

	int									r;					// zeroed in yyerror
} parse_param;

void bf_yyerror(void* loc, void* scanner, parse_param* pp, char const* err);

// bf parser; opaque type
struct bf_parser_t;
typedef struct bf_parser_t bf_parser;

/// bf_mkparser
//
// creates an instance of a bf parser which may be passed repeatedly to parse
//
#define bf_mkparser_onfail "p: %p"
int bf_mkparser(bf_parser ** const restrict p)
	__attribute__((nonnull));

/// bf_parse
//
// parse a buildfile
//
// parameters
//
//   p    - parser returned from mkparser
//   path - path to buildfile
//   bf   - results go here
//
// returns
//  0 on error - check *bf to see if the parse was successful
//
#define bf_parse_onfail "p: %p, path: '%s', bf: %p"
int bf_parse(
	  const bf_parser * const restrict p
	, char* path
	, bf_node ** const restrict bf
)
	__attribute__((nonnull));

/// bf_freeparser
//
// free a bf parser returned from mkparser with free semantics
//
void bf_freeparser(bf_parser* const restrict);

/// bf_xfreeparser
//
// free a bf parser returned from mkparser with xfree semantics
//
void bf_xfreeparser(bf_parser ** const restrict)
	__attribute__((nonnull));

/// bf_freenode
//
// free a bf_node with free semantics
//
void bf_freenode(bf_node * const restrict);

/// bf_xfreenode
//
// free a bf_node with xfree semantics
//
void bf_xfreenode(bf_node ** const restrict)
	__attribute__((nonnull));

#endif
