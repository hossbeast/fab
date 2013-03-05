#ifndef _LISTWISE_LSTACKH
#define _LISTWISE_LSTACKH

#include <listwise/generator.h>

///
/// [[ LSTACK API ]]
///

#define LSTACK_ITERATE_LOOP(ls, y, go)							\
	{																									\
		int go = 1;																			\
		if(!(ls)->sel.all)															\
		{																								\
			go = 0;																				\
			if((ls)->sel.sl > (y/8))											\
			{																							\
				go = ((ls)->sel.s[y/8] & (0x01 << (y%8)));	\
			}																							\
		}

#define LSTACK_ITERATE_HEADER(ls, x, y, go)	\
	for(y = 0; y < (ls)->s[x].l; y++)

#define LSTACK_ITERREV_HEADER(ls, x, y, go)	\
	for(y = (ls)->s[x].l - 1; y >= 0; y--)

// iterate the selected elements of the 0th list of the lstack
#define LSTACK_ITERATE(ls, y, go)				\
	LSTACK_ITERATE_HEADER(ls, 0, y, go)		\
	LSTACK_ITERATE_LOOP(ls, y, go)

// iterate in reverse the selected elements of the 0th list of the lstack
#define LSTACK_ITERREV(ls, y, go)				\
	LSTACK_ITERREV_HEADER(ls, 0, y, go)		\
	LSTACK_ITERATE_LOOP(ls, y, go)

// iterate the selected elements of the xth list on the lstack
#define LSTACK_ITERATE_LIST(ls, x, y, go) \
	LSTACK_ITERATE_HEADER(ls, x, y, go)			\
	LSTACK_ITERATE_LOOP(ls, y, go)

// iterate in reverse the selected elements of the xth list on the lstack
#define LSTACK_ITERREV_LIST(ls, x, y, go)	\
	LSTACK_ITERREV_HEADER(ls, x, y, go)			\
	LSTACK_ITERATE_LOOP(ls, y, go)

// close an iterate block
#define LSTACK_ITEREND }

/// lstack_exec
//
// if *r == 0, create it as the empty list stack
//
// then, executes the generator against *r
//
// parameters
//  g      - generator
//  init   - initial items to write at top of list stack
//  initls - lenghts of items in init, 0 for strlen
//  initl  - number of items in init
//  r      - input/output list stack
//
int lstack_exec(generator* g, char** init, int* initls, int initl, lstack** r);

/// lstack_dump
//
// print a list-stack to stdout
//
void lstack_dump(lstack*);

/// lstack_reset
//
// reset (but do not deallocate) a list stack
//
// no-op with zero-valued parameter
//
void lstack_reset(lstack*);

#endif
