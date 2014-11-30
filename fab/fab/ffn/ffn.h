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

#ifndef _FFN_H
#define _FFN_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#include "listwise.h"
#include "listwise/transform.h"

#include "pstring.h"

#include "yyutil.h"

#define restrict __restrict

// FFN flags
#define FFN_SINGLE			0x0001
#define FFN_MULTI				0x0002
#define FFN_DISCOVERY		0x0004
#define FFN_FABRICATION	0x0008
#define FFN_WEAK				0x0010
#define FFN_BRIDGE			0x0020
#define FFN_SUBCONTEXT	0x0040
#define FFN_WSSEP				0x0080
#define FFN_COMMASEP		0x0100
#define FFN_SYSVAR			0x0200

// FFN type table
#define FFN_TABLE(x)										\
	_FFN(FFN_STMTLIST				, 0x01	, x)	\
	_FFN(FFN_DEPENDENCY			, 0x02	, x)	\
	_FFN(FFN_FORMULA				, 0x03	, x)	\
	_FFN(FFN_INVOCATION			, 0x04	, x)	\
	_FFN(FFN_VARASSIGN			, 0x05	, x)	\
	_FFN(FFN_VARXFM_ADD			, 0x06	, x)	\
	_FFN(FFN_VARXFM_SUB			, 0x07	, x)	\
	_FFN(FFN_VARXFM_LW			, 0x08	, x)	\
	_FFN(FFN_VARLOCK				, 0x09	, x)	\
	_FFN(FFN_VARLINK				, 0x0a	, x)	\
	_FFN(FFN_LIST						, 0x0b	, x)	\
	_FFN(FFN_TRANSFORM			, 0x0c	, x)	\
	_FFN(FFN_VARREF					, 0x0d	, x)	\
	_FFN(FFN_LF							, 0x0e	, x)	\
	_FFN(FFN_WORD						, 0x0f	, x)	\
	_FFN(FFN_NOFILE					, 0x10	, x)	\
	_FFN(FFN_ONCEBLOCK			, 0x11	, x)

enum {
#define _FFN(a, b, c) a = b,
	FFN_TABLE(0)
#undef _FFN
};

#define _FFN(a, b, c) (c) == b ? #a "[" #b "]" :
#define FFN_STRING(x) FFN_TABLE(x) "unknown"

struct ff_file;
struct fml;

typedef struct ff_loc
{
	yyu_location;

	struct ff_file *	ff;
} __attribute__((packed)) ff_loc;

typedef struct ff_node
{
	uint32_t		type;					// node type
	ff_loc			loc;					// node location (cumulative)
//	char*			s;						// string value (cumulative)
//	int				l;						// length

	transform * transform;		// FFN_TRANSFORM
	uint32_t		flags;				// FFN_DEPENDENCY, FFN_FORMULA, FFN_INVOCATION, FFN_LIST, FFN_VARREF

	// not freed
	struct fml *			fml;		// FFN_FORMULA

	/*
	** strings are freed in freenode
	*/
	union {
		pstring * strings[1];

		struct {													// FFN_WORD
			pstring *			text;
		};
	};

	/*
	** nodes_owned are freed in freenode
	*/
	union {
		struct ff_node*			nodes_owned[2];

		struct {													// FFN_LIST, FFN_VARXFM
			struct ff_node*			transform_node;
			struct ff_node*			transform_list_node;
		};

		struct {													// FFN_VARREF
			struct ff_node *		name;
		};

		struct {													// FFN_VARASSIGN, FFN_VARLOCK, FFN_VARLINK, FFN_DESIGNATE
			struct ff_node*			definition;
		};

		struct {													// FFN_FORMULA
			struct ff_node*			targets_0;
			struct ff_node*			command;
		};

		struct {													// FFN_DEPENDENCY
			struct ff_node*			needs;
			struct ff_node*			feeds;
		};

		struct {													// FFN_INVOCATION
			struct ff_node*			module;
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
			struct ff_node**	list;			// FFN_TRANSFORM
			int								listl;
			int								lista;
		};

		struct {											// FFN_STMTLIST, FFN_ONCEBLOCK
			struct ff_node**	statements;
			int								statementsl;
		};

		struct {											// FFN_LIST
			struct ff_node**	elements;
			int								elementsl;
		};

		struct {											// FFN_INVOCATION
			struct ff_node**	varsettings;
			int								varsettingsl;
		};

		struct {											// FFN_VARLOCK, FFN_VARLINK
			struct ff_node**	vars;
			int								varsl;
		};

		struct {
			struct ff_node ** parts;		// FFN_NOFILE
			int								partsl;
		};
	};

	// implementation
	struct ff_node*		chain[1];		// chains for this node
	struct ff_node*		next;				// next sibling in parents chain
} ff_node;

///
/// [[ api ]]
///

/// (XAPI) ffn_mknode
//
// SUMMARY
//  create a new ff_node
//
// PARAMETERS
//  n    - new node returned here
//  loc  - location
//  type - node type
//
int ffn_mknode(ff_node ** const restrict n, const yyu_location * const restrict loc, uint32_t type, ...)
	__attribute__((nonnull(1, 2)));

/// ffn_addchain
//
// attach b to a's chain
//
ff_node* ffn_addchain(ff_node * restrict a, ff_node * const restrict b)
	__attribute__((nonnull));

/// ffn_free
//
// flatten, strmeasure, parse_transforms
//
int ffn_postprocess(ff_node * const ffn, struct ff_file * const ff, transform_parser * const gp)
	__attribute__((nonnull(2, 3)));

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
