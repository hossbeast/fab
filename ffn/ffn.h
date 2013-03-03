#ifndef _FFN_H
#define _FFN_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#include <listwise.h>
#include <listwise/generator.h>

#define restrict __restrict

// FFN flags
#define FFN_SINGLE			0x01
#define FFN_MULTI				0x02
#define FFN_DISCOVERY		0x04
#define FFN_FABRICATION	0x08
#define FFN_WEAK				0x10
#define FFN_GATED				0x20

// FFN type table
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

struct ff_file;

typedef struct ff_loc
{
	int				f_lin;
	int				f_col;
	int				l_lin;
	int				l_col;

	struct ff_file *	ff;
} __attribute__((packed)) ff_loc;

typedef struct ff_node
{
	uint32_t		type;		// node type
	ff_loc			loc;		// location of the node

	char*				s;			// string value of the entire node
	int					l;			// string length

	generator * 		generator;		// FFN_GENERATOR
	uint8_t					flags;				// FFN_DEPENDENCY, FFN_FORMULA, FFN_INVOCATION

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
			int								listl;
			int								lista;
		};

		struct {											// FFN_STMTLIST
			struct ff_node**	statements;
			int								statementsl;
		};

		struct {											// FFN_LIST
			struct ff_node**	elements;
			int								elementsl;
		};

		struct {											// FFN_FORMULA
			struct ff_node**	commands;
			int								commandsl;
		};

		struct {											// FFN_INVOCATION
			struct ff_node**	designations;
			int								designationsl;
		};

		struct {											// FFN_DESIGNATE
			struct ff_node**	vars;
			int								varsl;
		};
	};

	/*
	** xten is not freed
	*/
	union {
		void *			xten;

		void *			fml;						// FFN_FORMULA
	};

	// implementation
	char*							e;
	struct ff_node*		chain[1];		// chains for this node
	struct ff_node*		next;				// next sibling in parents chain
} ff_node;

///
/// [[ api ]]
///

/// ffn_mknode
//
// free a ff_node with free semantics
//
ff_node* ffn_mknode(const void * const restrict loc, size_t locz, struct ff_file * const restrict ff, uint32_t type, ...)
	__attribute__((nonnull(1, 3)));

/// ffn_addchain
//
// attach b to a's chain
//
ff_node* ffn_addchain(ff_node * restrict a, ff_node * const restrict b)
	__attribute__((nonnull));

/// ffn_free
//
// flatten, strmeasure, parse_generators
//
int ffn_postprocess(ff_node * const ffn, generator_parser * const gp)
	__attribute__((nonnull));

/// ffn_free
//
// free a ff_node with free semantics
//
void ffn_free(ff_node * const restrict);

/// ffn_xfreenode
//
// free a ff_node with xfree semantics
//
void ffn_xfree(ff_node ** const restrict)
	__attribute__((nonnull));

/// ffn_dump
//
// log an ff_node under FFTREE
//
void ffn_dump(ff_node * const restrict root)
	__attribute__((nonnull));

#undef restrict
#endif
