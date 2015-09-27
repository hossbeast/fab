/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LISTWISE_OPERATORS_H
#define _LISTWISE_OPERATORS_H

#include "xapi.h"

#define restrict __restrict

struct operator;

/// listwise_register_opdir
//
// SUMMARY
//  register an application-specific directory as containing compiled operator .so's
//  in addition to the default locations liblistwise loads these from
//
xapi listwise_register_opdir(char * const restrict dir)
	__attribute__((nonnull));

// available operators - dloaded at init time
extern struct operator** 		g_ops;
extern struct operator** 		g_ops_by_s;					// pointers to g_ops sorted by s
extern struct operator** 		g_ops_by_mnemonic;	// pointers to g_ops sorted by mnemonic
extern int									g_ops_l;

// lookup an operator by name or mnemonic
struct operator*						op_lookup(char* s, int l);

// lookup an operator by name
struct operator*						op_lookup_by_s(char* s, int l);

// lookup an operator by mnemonic
struct operator*						op_lookup_by_mnemonic(char* s, int l);

#undef restrict
#endif
