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

#include "listwise.h"
#include "listwise/operator.h"
#include "listwise/generator.h"
#include "listwise/lstack.h"
#include "listwise/ops.h"
#include "listwise/object.h"

#include "coll.h"
#include "idx.h"

#define API __attribute__((visibility("protected")))
#define APIDATA

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

		idx * by_type;					// indexed by type
	};
} object_registry;

#endif
