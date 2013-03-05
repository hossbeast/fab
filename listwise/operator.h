#ifndef _LISTWISE_OPERATOR_H
#define _LISTWISE_OPERATOR_H

#include <stdint.h>
#include <stdio.h>

#include <pcre.h>

#include <listwise.h>

#define restrict __restrict

/*
** LISTWISE OPERATOR API
**
*/

struct operation;
struct operator;
struct arg;

//
// operators are of one of these types
//
enum
{
	  LWOP_SELECTION_READ					= 0x0001			// uses the current selection
	, LWOP_SELECTION_WRITE				= 0x0002			// implicitly apply the y operator after this operator
	, LWOP_SELECTION_RESET				= 0x0004			// resets the current selection
	, LWOP_MODIFIERS_CANHAVE			= 0x0008			// last argument is a modifiers string
	, LWOP_ARGS_CANHAVE						= 0x0010			// can have arguments
	, LWOP_OPERATION_PUSHBEFORE		= 0x0020			// first operation is to push an empty list
	, LWOP_OPERATION_INPLACE			= 0x0040			// modifies the string in the top list in-place
	, LWOP_OPERATION_FILESYSTEM		= 0x0080			// filesystem operator
	, LWOP_OBJECT_NO							= 0x0100			// operator ignores object entries
	, LWOP_EMPTYSTACK_YES					= 0x0200			// operator invoked even in the context of an empty stack
};

//
// operator - each op exports one of these structs
//
typedef struct operator
{
	void *		handle;		// dlopen handle
	uint64_t	optype;		// OPTYPE_*
	char*			desc;			// operator description

	// methods
	int 		(*op_validate)(struct operation*);
	int 		(*op_exec)(struct operation*, lstack*, int**, int*);

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
int lstack_append(lstack * const restrict ls, int x, int y, const char* const restrict s, int l)
	__attribute__((nonnull));

/// lstack_appendf
//
// append text to the entry at x:y using printf-style args
//
int lstack_appendf(lstack* const restrict ls, int x, int y, const char* const restrict s, ...)
	__attribute__((nonnull));

/// lstack_write
//
// write text to the entry at x:y
//
int lstack_write(lstack* const restrict ls, int x, int y, const char* const restrict s, int l)
	__attribute__((nonnull));

int lstack_write_alt(lstack* const restrict ls, int x, int y, const char* const restrict s, int l)
	__attribute__((nonnull));

/// lstack_writef
//
// write text to the entry at x:y using printf-style args
//
int lstack_writef(lstack* const restrict ls, int x, int y, const char* const restrict s, ...)
	__attribute__((nonnull));

/// lstack_add
//
// write text to the next unused entry of list 0
//
int lstack_add(lstack* const restrict ls, const char* const restrict s, int l)
	__attribute__((nonnull));

/// lstack_addf
//
// write text to the next unused entry of list 0 using printf-style args
//
int lstack_addf(lstack* const restrict ls, const char* const restrict fmt, ...)
	__attribute__((nonnull));

/// lstack_shift
//
// delete the 0th list - all other lists are shifted down
//
int lstack_shift(lstack* const restrict ls)
	__attribute__((nonnull));

/// lstack_unshift
//
// unshift an empty list onto the stack at index zero - all other lists are shifted up
//
int lstack_unshift(lstack* const restrict ls)
	__attribute__((nonnull));

/// lstack_pop
//
// delete the Nth list
//
int lstack_pop(lstack* const restrict ls)
	__attribute__((nonnull));

/// lstack_push
//
// push an empty list onto the end of the stack
//
int lstack_push(lstack* const restrict ls)
	__attribute__((nonnull));

/// lstack_cycle
//
// move the first list on the stack to the last position
//
int lstack_cycle(lstack* const restrict ls)
	__attribute__((nonnull));

/// lstack_recycle
//
// move the last list on the stack to the first position
//
int lstack_recycle(lstack* const restrict ls)
	__attribute__((nonnull));

/// lstack_xchg
//
// exchange the first and second lists on the stack
//
int lstack_xchg(lstack* const restrict ls)
	__attribute__((nonnull));

/// lstack_merge
//
//
//
int lstack_merge(lstack* const restrict ls, int a, int b)
	__attribute__((nonnull));

/// lstack_string
//
// get pointer/length of a string for the entry at the specified position
//
int lstack_string(lstack* const restrict ls, int x, int y, char ** r, int * rl);

/// lstack_getstring
//
// get a null-terminated string for the entry at the specified position
//
//  ** use lstack_string wherever possible, this function consumes tmp space **
//
char* lstack_getstring(lstack* const restrict ls, int x, int y);

/// allocate
//
//
//
int lstack_allocate(lstack* const restrict ls, int x, int y, int z);

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
int lstack_ensure(lstack* const restrict ls, int x, int y, int z);

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
//  lstack_move returns 1
//
int lstack_move(lstack* const restrict ls, int ax, int ay, int bx, int by)
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
int lstack_delete(lstack * const restrict ls, int x, int y)
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
//  0 on error, 1 otherwise
//
int lstack_displace(lstack * const restrict ls, int x, int y, int l)
	__attribute__((nonnull));

/// sel_clear
//
// clear the selection
//
int lstack_sel_clear(lstack* const restrict ls)
	__attribute__((nonnull));

/// sel_set
//
// select the entry at 0:y
//
int lstack_sel_set(lstack* const restrict ls, int y)
	__attribute__((nonnull));

/// sel_write
//
// select exactly those entries specified in news
//
// PARAMETERS
//  ls    - list-stack
//  news  - bitvector specifying the selection
//  newsl - elements in news
//
int lstack_sel_write(lstack* const restrict ls, uint8_t * news, int newsl)
	__attribute__((nonnull));

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

/// listwise_sanity
//
// cause lstack_exec* family of functions to perform sanity checks on ls before
// beginning, after every operator completes, and upon completion
//
// if a sanity check fails, further use of ls will cause the program to crash or
// memory will be lost
//
// if a sanity check fails, print error(s) to listwise_err_fd and call exit(1)
//
// DEFAULT
//  0 - no sanity checks
//
extern int listwise_sanity;

/// listwise_allocation_seed
//
// SUMMARY
//  initial size for new stack, list, and string allocations
//
// DEFAULT
//  10
//
extern int listwise_allocation_seed;

#endif
