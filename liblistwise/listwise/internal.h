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

#ifndef _LISTWISE_INTERNAL_H
#define _LISTWISE_INTERNAL_H

// liblistwise api
#include "listwise.h"
#include "listwise/xtra.h"

// liblistwise operator api
#include "listwise/operator.h"
#include "listwise/lwx.h"
#include "listwise/generator.h"
#include "listwise/ops.h"
#include "listwise/object.h"
#include "listwise/re.h"

#include "coll.h"
#include "idx.h"

#define API __attribute__((visibility("protected")))
#define APIDATA

#define restrict __restrict

// collection of registered object types
// with lookup index by type id
extern union object_registry_t
{
	coll_doubly c;

	struct
	{
		int l;
		int a;
		int z;

		struct listwise_object ** e;		// object defs

		idx * by_type;									// indexed by type
	};
} object_registry;

#define GENSCAN_MODE								0x00F
#define GENSCAN_MODE_WITHREFS				0x000
#define GENSCAN_MODE_NOREFS					0x001

#define GS_REFS(x) (((x) & GENSCAN_MODE) == GENSCAN_MODE_REFS)

#define GENSCAN_CLOSURE							0x0F0
#define GENSCAN_CLOSURE_OPEN				0x000
#define GENSCAN_CLOSURE_CLOSED			0x010

#define GS_CLOSED(x) (((x) & GENSCAN_CLOSURE) == GENSCAN_CLOSURE_CLOSED)

#define GENSCAN_CHAR								0xFF0
#define GENSCAN_CHAR_SLASH					(0x000 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_CHAR_COMMA					(0x100 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_CHAR_BRACES					(0x200 | GENSCAN_CLOSURE_CLOSED)

// table of genscan modes
#define GENSCAN_TABLE(x) 																																																	\
  _GENSCAN(GENSCAN_SLASH_NOREFS			, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_SLASH	, '/'	, '/'	, slash		,	norefs		, x)	\
  _GENSCAN(GENSCAN_SLASH_WITHREFS		, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_SLASH	, '/'	, '/'	, slash		,	withrefs	, x)	\
  _GENSCAN(GENSCAN_COMMA_NOREFS			, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_COMMA	, ','	, ','	, comma		,	norefs		, x)	\
  _GENSCAN(GENSCAN_COMMA_WITHREFS		, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_COMMA	, ','	, ','	, comma		,	withrefs	, x)	\
  _GENSCAN(GENSCAN_BRACES_NOREFS		, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_BRACES	, '{'	, '}'	, braces	,	norefs		, x)	\
  _GENSCAN(GENSCAN_BRACES_WITHREFS	, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_BRACES	, '{'	, '}'	, braces	,	withrefs	, x)	\

// declare genscan modes
enum {
#define _GENSCAN(a, b, c, d, e, f, g) a = b,
GENSCAN_TABLE(0)
#undef _GENSCAN
};

// indexed by genscan mode, yields opening char
char * genscan_opener;

// indexed by genscan mode, yields closing char
char * genscan_closer;

// indexed by genscan mode, yields base state for that scanmode
int * genscan_basestate;

// indexed by genscan mode, yields arg parsing state for that scanmode
int * genscan_argstate;

#define _GENSCAN(a, b, c, d, e, f, g) g == b ? #a :
#define GENSCAN_STR(x) GENSCAN_TABLE(x) "GENSCAN_UNKNWN"

/// generator_operation_write
//
// SUMMARY
//  write a string describing an operation in a generator to a buffer
//
// PARAMETERS
//  oper       - operation
//  buf        - buffer
//  sz         - max bytes to write to buffer, including null byte
//  [scanmode] - scanning mode, one of GENSCAN_*
//
// RETURNS
//  number of bytes wriitten to buf
//
size_t generator_operation_write(const operation * const restrict oper, char * restrict buf, const size_t sz, uint32_t * scanmode)
	__attribute__((nonnull(1, 2)));

#undef restrict
#endif
