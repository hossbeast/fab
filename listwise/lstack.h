/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _LISTWISE_LSTACKH
#define _LISTWISE_LSTACKH

#include <listwise/generator.h>

#define LSTACK_ITERATE_LOOP(ls, y, go)						\
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
	if((ls)->l > x)														\
	{																					\
		for(y = 0; y < (ls)->s[x].l; y++)				\
		{

#define LSTACK_ITERREV_HEADER(ls, x, y, go)	\
	if((ls)->l > x)														\
	{																					\
		for(y = (ls)->s[x].l - 1; y >= 0; y--)	\
		{

///
/// [[ LSTACK API ]]
///

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
#define LSTACK_ITEREND }}

#define LSTACK_COUNT(ls) ({		\
	int c = 0;									\
	int x;											\
	LSTACK_ITERATE(ls, x, go)		\
	if(go)											\
	{														\
		c++;											\
	}														\
	LSTACK_ITEREND;							\
	c;													\
})

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





