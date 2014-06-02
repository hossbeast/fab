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
// operators are of one of these types
//
enum
{
/* these actually have some effect on processing */
	  LWOP_SELECTION_STAGE				= 0x0001			// stages selections (which may be activated with the y operator)
	, LWOP_SELECTION_ACTIVATE			= 0x0002 | LWOP_SELECTION_STAGE			//  lw applies the y operator after this operator
	, LWOP_SELECTION_RESET				= 0x0004			// lw resets the selection after executing the operator (apart from the staging mechanism)
	, LWOP_WINDOWS_STAGE					= 0x0008			// stages windows (which may be ratified with the y operator)
	, LWOP_WINDOWS_ACTIVATE				= 0x0010 | LWOP_WINDOWS_STAGE				//  lw applies the y operator after this operator
	, LWOP_WINDOWS_RESET					= 0x0020			// lw resets all windows after executing this operator (apart from the staging mechanism)
	, LWOP_ARGS_CANHAVE						= 0x0040			// can have arguments (verified before op_validate)
	, LWOP_EMPTYSTACK_YES					= 0x0080			// operator is invoked even in the context of an empty stack

/* there are informational */
	, LWOP_STACKOP								= 0x0100 | LWOP_SELECTION_RESET | LWOP_WINDOWS_RESET			// stack operation : manipulation of entire lists
	, LWOP_MODIFIERS_CANHAVE			= 0x0200			// last argument is a modifiers string
	, LWOP_OPERATION_PUSHBEFORE		= 0x0480 | LWOP_STACKOP																	// first operation is to push an empty list
	, LWOP_OPERATION_INPLACE			= 0x0800			// modifies the contents of rows in the top list in-place
	, LWOP_OPERATION_FILESYSTEM		= 0x1000			// operator reads from the filesystem
};

//
// operator - each op exports one of these structs
//
typedef struct operator
{
	uint64_t	optype;		// OPTYPE_*
	char *		desc;			// operator description
	char *		mnemonic;	// longform name

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

	char		s[6];			// name; overwritten by liblist during initialization
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
