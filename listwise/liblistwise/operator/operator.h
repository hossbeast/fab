/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LISTWISE_OPERATOR_H
#define _LISTWISE_OPERATOR_H

#include <stdint.h>
#include <stdio.h>

#include "pstring.h"

#include "listwise.h"
#include "listwise/lstack.h"
#include "listwise/LW.errtab.h"
//#include "listwise/PCRE.errtab.h"
#include "listwise/selection.h"
#include "listwise/window.h"

#include <pcre.h>

#define restrict __restrict

/*
** listwise operator API
**
** CORE
**  operator.h           - structure definitions
**  lwx.h                - lwx definition
**  lstack.h             - manipulating lstacks
**  selection.h          - manipulating the selection
**  window.h             - manipulating windows
**  LW.errtab.h          - liblistwise error table
**  PCRE.errtab.h        - libpcre error table
**
** extra functionality
**  listwise/re.h        - libpcre wrappers
**  listwise/iterate.h   - iterating lstack contents
**  listwise/object.h    - manipulating object entries
**  listwise/ops.h       - enumerating and looking up listwise operators
**  listwise/generator.h - manipulating generators
**  listwise/fs.h        - filesystem apis
**  listwise/log.h       - logging functions
*/

struct operation;
struct operator;
struct arg;

//
// operator modifiers
//

/* these actually have some effect on processing */
#define LWOP_SELECTION_STAGE				LWOPT_SELECTION_STAGE						// stages selections (which may be activated with the y operator)
#define LWOP_SELECTION_ACTIVATE			(LWOPT_SELECTION_STAGE	| LWOPT_SELECTION_ACTIVATE)			//  lw applies the sy operator after this operator
#define LWOP_SELECTION_RESET				LWOPT_SELECTION_RESET						// lw resets the selection after executing the operator (apart from the staging mechanism)
#define LWOP_SELECTION_AGGREGATE		LWOPT_SELECTION_AGGREGATE
#define LWOP_WINDOWS_STAGE					LWOPT_WINDOWS_STAGE							// stages windows (which may be ratified with the y operator)
#define LWOP_WINDOWS_ACTIVATE				(LWOPT_WINDOWS_STAGE		| LWOPT_WINDOWS_ACTIVATE)				//  lw applies the wy operator after this operator
#define LWOP_WINDOWS_RESET					LWOPT_WINDOWS_RESET							// lw resets all windows after executing this operator (apart from the staging mechanism)
#define LWOP_WINDOWS_AGGREGATE			LWOPT_WINDOWS_AGGREGATE			
#define LWOP_ACTIVATION_OVERRIDE		LWOPT_ACTIVATION_OVERRIDE
#define LWOP_ARGS_CANHAVE						LWOPT_ARGS_CANHAVE							// can have arguments (verified before op_validate)
#define LWOP_EMPTYSTACK_YES					LWOPT_EMPTYSTACK_YES						// operator is invoked even in the context of an empty stack

// bits <= LWOP_EFFECTUAL have an effect
#define LWOP_EFFECTUAL LWOP_EMPTYSTACK_YES

/* there are informational */
#define LWOP_STACKOP								LWOPT_STACKOP								| LWOPT_SELECTION_RESET | LWOPT_WINDOWS_RESET		// stack operation : manipulation of entire lists
#define LWOP_MODIFIERS_CANHAVE			LWOPT_MODIFIERS_CANHAVE					// last argument is a modifiers string
#define LWOP_OPERATION_PUSHBEFORE		LWOPT_OPERATION_PUSHBEFORE	| LWOP_STACKOP																	// first operation is to push an empty list
#define LWOP_OPERATION_INPLACE			LWOPT_OPERATION_INPLACE					// modifies the contents of rows in the top list in-place
#define LWOP_OPERATION_FILESYSTEM		LWOPT_OPERATION_FILESYSTEM			// operator reads from the filesystem

/* the individual options are defined here */
#define LWOPT_TABLE(x)													\
	_LWOPT(SELECTION_STAGE				, 0x0001	, x)	\
	_LWOPT(SELECTION_ACTIVATE			, 0x0002	, x)	\
	_LWOPT(SELECTION_RESET				, 0x0004	, x)	\
	_LWOPT(SELECTION_AGGREGATE		, 0x0008	, x)	\
	_LWOPT(WINDOWS_STAGE					, 0x0010	, x)	\
	_LWOPT(WINDOWS_ACTIVATE				, 0x0020	, x)	\
	_LWOPT(WINDOWS_RESET					, 0x0040	, x)	\
	_LWOPT(WINDOWS_AGGREGATE			, 0x0080	, x)	\
	_LWOPT(ACTIVATION_OVERRIDE		, 0x0100	, x)	\
	_LWOPT(ARGS_CANHAVE						, 0x0200	, x)	\
	_LWOPT(EMPTYSTACK_YES					, 0x0400	, x)	\
	_LWOPT(STACKOP								, 0x0800	, x)	\
	_LWOPT(MODIFIERS_CANHAVE			, 0x1000	, x)	\
	_LWOPT(OPERATION_PUSHBEFORE		, 0x2000	, x)	\
	_LWOPT(OPERATION_INPLACE			, 0x4000	, x)	\
	_LWOPT(OPERATION_FILESYSTEM		, 0x8000	, x)
	
enum {
#define _LWOPT(a, b, x) LWOPT_ ## a = b,
LWOPT_TABLE(0)
#undef _LWOPT
};

#define _LWOPT(a, b, x) (x) == (b) ? #a :
#define LWOPT_STR(x) LWOPT_TABLE(x) "LWOPT_UNKNWN"

/// listwise_lwop_write
//
// SUMMARY
//  write a description of the specified optype to the specified buffer
//
// PARAMETERS
//  optype    - 
//  effectual - whether to include only effectual bits and ignore informational-only bits
//  [dst]     - 
//  [sz]      -
//  [z]       - 
//  [ps]      - 
//  writer    - 
//
xapi listwise_lwop_write(uint64_t optype, int effectual, char * const restrict dst, const size_t sz, size_t * restrict z)
	__attribute__((nonnull(3)));

/// listwise_lwop_pswrite
//
// SUMMARY
//  write a description of the specified optype to the specified pstring
//
// PARAMETERS
//  optype    - 
//  effectual - whether to include only effectual bits and ignore informational-only bits
//  [dst]     - 
//  [sz]      -
//  [z]       - 
//  [ps]      - 
//  writer    - 
//
xapi listwise_lwop_pswrite(uint64_t optype, int effectual, pstring ** const restrict ps)
	__attribute__((nonnull));

//
// operator - each op exports one of these structs
//
typedef struct operator
{
	uint64_t	optype;			// bitwise combination of LWOP_*
	char *		desc;				// operator description
	char *		mnemonic;		// longform name ; not required
	int				mnemonicl;	// mnemonic length

	/// op_validate
	//
	// SUMMARY
	//  validate the operation
	//
	int 		(*op_validate)(struct operation*);

	/// op_exec
	//
	// SUMMARY
	//  execute operator
	//
	// PARAMETERS
	//  o        - operation
	//  lx       - lstack
	//  ovec     - (see listwise/re.h)
	//  ovec_len -
	//  udata    - (see listwise/log.h)
	//
	int 		(*op_exec)(struct operation*, lwx*, int**, int*, void**);

	char		s[6];			// name ; required
	int			sl;				// name length
} operator;

//
// references which occur within arguments are of one of these types
//
enum
{
	  REFTYPE_BREF = 10
};

//
// argument
//
typedef struct arg
{
	char*		s;		// string value of the arg, null-terminated
	int			l;		// string length

	struct
	{
		struct ref		// references within the string
		{
			char* s;		// start of the reference 
			char* e;		//   end of the reference (pointer to the character following the last character of the reference)
			int		l;		// length of the reference (l = e - s)

			int		k;		// REFTYPE_*

			union
			{
				int		ref;		// value of the backreference
			};
		}				*v;

		int			l;	// number of references
		int			a;	// alloc

		// pointer to the last reference, if any
		struct ref * last;
	} refs;

#define ITYPE_RE		1
#define ITYPE_I64		2

	// indicates which member of the interpreted value union to use
	uint8_t						itype;

	union					// interpreted value of the arg, if any
	{
		struct re
		{
			pcre*					c_pcre;
			pcre_extra*		c_pcre_extra;
			int						c_caps;
		}								re;

		int64_t					i64;
	};
} arg;

//
// an operation consists of an operator and a set of arguments
//
typedef struct operation
{
	struct operation*	next;

	operator*					op;			// operator
	arg**							args;		// arguments
	int								argsl;
} operation;

#undef restrict
#endif
