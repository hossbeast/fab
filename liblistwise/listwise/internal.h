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

/// generator_operations_write
//
// SUMMARY
//  write a string describing a sequence of operations in a generator to a buffer
//
// PARAMETERS
//  ops        - operation list
//  opsl       - operations len
//  buf        - buffer
//  sz         - max bytes to write to buffer, including null byte
//  [scanmode] - scanning mode, one of GENSCAN_*
//
// RETURNS
//  number of bytes wriitten to buf
//
size_t generator_operations_write(operation ** const ops, int opsl, char * const restrict buf, const size_t sz, uint32_t * sm)
	__attribute__((nonnull(1, 3)));

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
size_t generator_operation_write(operation * const restrict oper, char * restrict buf, const size_t sz, uint32_t * scanmode)
	__attribute__((nonnull(1, 2)));

/// generator_args_write
//
// SUMMARY
//  write a string describing a sequence of arguments to a buffer
//
// PARAMETERS
//  args       - args list
//  argsl      - args len
//  buf        - buffer
//  sz         - max bytes to write to buffer, including null byte
//  [scanmode] - scanning mode, one of GENSCAN_*
//
// RETURNS
//  number of bytes wriitten to buf
//
size_t generator_args_write(arg ** const restrict args, const int argsl, char * const restrict buf, const size_t sz, uint32_t * sm)
	__attribute__((nonnull(1, 3)));

/// generator_arg_write
//
// SUMMARY
//  write a string describing an argument to a buffer
//
// PARAMETERS
//  args       - args list
//  argsl      - args len
//  buf        - buffer
//  sz         - max bytes to write to buffer, including null byte
//  [scanmode] - scanning mode, one of GENSCAN_*
//
// RETURNS
//  number of bytes wriitten to buf
//
size_t generator_arg_write(arg * const arg, char * const restrict buf, const size_t sz, uint32_t * sm)
	__attribute__((nonnull(1, 2)));

#undef restrict
#endif
