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

#ifndef _LISTWISE_OPERATOR_H
#define _LISTWISE_OPERATOR_H

#include <stdint.h>
#include <stdio.h>

#include "listwise.h"

#include <pcre.h>

#define restrict __restrict

/*
** LISTWISE OPERATOR API
**
**  listiwse/operator.h  - core operator functionality
**  listwise/lwx.h       - lwx definition
**   extra functionality
**  listwise/object.h    - manipulating object entries
**  listwise/ops.h       - enumerating and looking up listwise operators
**  listwise/generator.h - manipulating generators
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

	// methods
	int 		(*op_validate)(struct operation*);
	int 		(*op_exec)(struct operation*, lwx*, int**, int*);

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

	struct ref		// references within the string
	{
		char* s;		// start of the reference 
		char* e;		//   end of the reference (pointer to the character following the last character of the reference)
		int		l;		// length of the reference (l = e - s)

		int		k;		// REFTYPE_*

		union
		{
			int		bref;		// for REFTYPE_BREF, value of the backreference
		};
	}				*refs;
	int			refsl;	// number of references

	// pointer to the last reference, if any
	struct ref* ref_last;

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

/// lstack_append
//
// append text to the entry at x:y
//
int lstack_append(lwx * const restrict lx, int x, int y, const char* const restrict s, int l)
	__attribute__((nonnull));

/// lstack_appendf
//
// append text to the entry at x:y using printf-style args
//
int lstack_appendf(lwx * const restrict lx, int x, int y, const char* const restrict s, ...)
	__attribute__((nonnull));

/// lstack_write
//
// write text to the entry at x:y
//
int lstack_write(lwx * const restrict lx, int x, int y, const char* const restrict s, int l)
	__attribute__((nonnull));

int lstack_write_alt(lwx * const restrict lx, int x, int y, const char* const restrict s, int l)
	__attribute__((nonnull));

/// lstack_writef
//
// write text to the entry at x:y using printf-style args
//
int lstack_writef(lwx * const restrict lx, int x, int y, const char* const restrict s, ...)
	__attribute__((nonnull));

/// lstack_add
//
// write text to the next unused entry of list 0
//
int lstack_add(lwx * const restrict lx, const char* const restrict s, int l)
	__attribute__((nonnull));

/// lstack_addf
//
// write text to the next unused entry of list 0 using printf-style args
//
int lstack_addf(lwx * const restrict lx, const char* const restrict fmt, ...)
	__attribute__((nonnull));

///
/// STACKOPS API
///
///  typically after performing one of these operations, selection and/or windows need to be
///  reset or adjusted
///

/// lstack_shift
//
// delete the 0th list - all other lists are shifted down
//
int lstack_shift(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_unshift
//
// unshift an empty list onto the stack at index zero - all other lists are shifted up
//
int lstack_unshift(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_pop
//
// delete the Nth list
//
int lstack_pop(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_push
//
// push an empty list onto the end of the stack
//
int lstack_push(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_cycle
//
// move the first list on the stack to the last position
//
int lstack_cycle(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_recycle
//
// move the last list on the stack to the first position
//
int lstack_recycle(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_xchg
//
// exchange the first and second lists on the stack
//
int lstack_xchg(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_merge
//
// SUMMARY
//  append the rows in list b to the end of list a
//
int lstack_merge(lwx * const restrict lx, int a, int b)
	__attribute__((nonnull));

/// allocate
//
//
//
int lstack_allocate(lwx * const restrict lx, int x, int y, int z);

/// lstack_clear
//
// SUMMARY
//  reset the entry x:y
//
void lstack_clear(const lwx * const restrict lx, int x, int y);

/// ensure
//
// ensure stack/list allocation up to the specified dimensions
//
// parameters
//   x - zero based list index       ( or -1 to skip stack allocation)
//   y - zero based string index     ( or -1 to skip list allocation)
//   z - zero based character index  ( or -1 to skip string allocation)
//
// updates stack and list length to be at least the dimension specified
//  ** this is NOT done for string length **
//
int lstack_ensure(lwx * const restrict lx, int x, int y, int z);

/// lstack_move
//
// SUMMARY
//  move the entry at bx:by to position ax:ay
//
// DETAILS
//  1. copy ax:ay
//  2. set ax:ay = bx:by
//  3. copy entries following bx:by down by 1, overwriting it
//  4. place the original ax:ay at the end of ls->s[bx]
//  5. ls->s[bx].l--
//
// RETURNS
//  lstack_move returns 0
//
int lstack_move(lwx * const restrict lx, int ax, int ay, int bx, int by)
	__attribute__((nonnull));

/// lstack_delete
//
// SUMMARY
//  delete the entry at position x:y
//
// DETAILS
//  1. copy x:y
//  2. copy entries following x:y down by 1, overwriting it
//  3. place the original x:y at the end of ls->s[x]
//  4. ls->s[x].l--
//
// RETURNS
//  lstack_delete returns 1
//
int lstack_delete(lwx * const restrict lx, int x, int y)
	__attribute__((nonnull));

/// lstack_displace
//
// SUMMARY
//  prepare l entries at x:y to receive new writes
//  these entries should be immediately populated with lstack_write_alt/lstack_obj_write_alt
//
// DETAILS
//	1. ensure s[x] has s[x].l + l allocated entries
//  2. copy l unused entries from s[x].s[s[x].l]
//  3. copy l entries x:y -> x:y+l
//	4. copy l unused entries into x:y
//  5. s[x].l += l
//
// NOTES
//  no-op when l == 0
//
// RETURNS
//  1 on error
//
int lstack_displace(lwx * const restrict lx, int x, int y, int l)
	__attribute__((nonnull));

///
/// SELECTION API
///

/// lstack_sel_all
//
// SUMMARY
//  select all entries
//
int lstack_sel_reset(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_sel_stage
//
// SUMMARY
//  stage the selection 0:y
//
int lstack_sel_stage(lwx * const restrict lx, int y)
	__attribute__((nonnull));

/// lstack_sel_unstage
//
// SUMMARY
//  unstage staged selections
//
int lstack_sel_unstage(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_sel_activate
//
// SUMMARY
//  activate selections staged by the previous operation
//
int lstack_sel_activate(lwx * const restrict lx)
	__attribute__((nonnull));

///
/// WINDOWING API
///

/// lstack_window_stage
//
// SUMMARY
//  add a segment to the staged window for the specified row
//
int lstack_window_stage(lwx * const restrict lx, int y, int off, int len)
	__attribute__((nonnull));

/// lstack_window_reset
//
// SUMMARY
//  reset the window for the specified row
//
int lstack_window_reset(lwx * const restrict lx, int y)
	__attribute__((nonnull));

/// lstack_windows_unstage
//
// SUMMARY
//  unstage staged windows
//
int lstack_windows_unstage(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_windows_reset
//
// SUMMARY
//  deactivate windows
//
int lstack_windows_reset(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_windows_activate
//
// SUMMARY
//  activate staged windows
//
int lstack_windows_activate(lwx * const restrict lx)
	__attribute__((nonnull));

///
/// RE API
///

/// re_compile
//
// compile the re for an op arg representing a regex
//
// parameters
//  re  - regex op arg
//  mod - modifier string arg, if any
//
int re_compile(char* s, struct re* re, char* mod);

/// re_exec
//
// execute an regex against a string
//
// parameters
//  re       - regex already compiled with re_compile
//  s        - subject string
//  l        - length of s
//  o        - offset from s to start matching
//  ovec     - results and subcapture information written to *ovec
//  ovec_len - *ovec size is at *ovec_len
//
// returns 1 on success, 0 on failure (memory allocation, bad pcre, etc)
//
// example
//    char* s;
//    int l;
//    int* ovec;
//    int ovec_len;
//		if(re_exec(re, s, l, 0, &ovec, &ovec_len) == 0)
//		{
//			/* failure */
//		}
//		else if(ovec[0] > 0)
//		{
//			int x;
//			for(x = 0; x < ovec[0]; x++)
//			{
//				int a = ovec[1 + (x * 2) + 0];
//				int b = ovec[1 + (x * 2) + 1];
//
//				if(a >= 0 && b >= 0)
//				{
//					/* print x'th subcapture - recall the 0th subcapture is the entire match */
//					printf("%.*s", b - a, s + a);
//				}
//			}
//		}
//		else
//		{
//			/* no match */
//		}
//
int re_exec(struct re* re, char* s, int l, int o, int** ovec, int* ovec_len);

/// lstack_readrow
//
// SUMMARY
//
// PARAMETERS
//  lx    - lw context
//  x     - list offset
//  y     - row offset
//  r     - return value - ptr
//  rl    - return value - length
//  [obj] - whether object references are resolved
//  [win] - whether windowing is resolved
//  [str] - whether the return value is required to be null-terminated and in tmp space
//  [raw] - indicates whether the raw storage was returned (object entry not resolved, window not resolved, and str was not specified)
//
int lstack_readrow(lwx * const restrict lx, int x, int y, char ** const restrict r, int * const restrict rl, int obj, int win, int str, int * const restrict raw)
	__attribute__((nonnull(1, 4, 5)));

/// listwise_allocation_seed
//
// SUMMARY
//  initial size for new stack, list, and string allocations
//
// DEFAULT
//  10
//
extern int listwise_allocation_seed;

#undef restrict
#endif
