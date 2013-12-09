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

#include <stddef.h>

#include "listwise/internal.h"

#include "coll.h"
#include "idx.h"


union object_registry_t object_registry = { { .size = sizeof(listwise_object*) } };

static void __attribute__((destructor)) teardown()
{
	int x;
	for(x = 0; x < object_registry.l; x++)
		free(object_registry.e[x]->string_property);

	free(object_registry.e);

	idx_free(object_registry.by_type);
}

///
/// [[ LSTACK API (for objects) ]]
///

int API listwise_register_object(uint8_t type, listwise_object * def)
{
	def->type = type;

	// add this type to the collection
	fatal(coll_doubly_add, &object_registry.c, &def, 0);

	// reindex
	fatal(idx_mkindex
		, object_registry.e
		, object_registry.l
		, object_registry.z
		, offsetof(typeof(object_registry.e[0][0]), type)
		, sizeof(((typeof(object_registry.e[0][0])*)0)->type)
		, INDEX_UNIQUE | INDEX_NUMERIC | INDEX_DEREF
		, &object_registry.by_type
	);

	finally : coda;
}

int API listwise_enumerate_objects(listwise_object *** list, int * list_len)
{
	return idx_enumerate(object_registry.by_type, list, list_len);
}

int API listwise_lookup_object(uint8_t type, listwise_object ** obj)
{
	if(((*obj) = idx_lookup_val(object_registry.by_type, &type, 0)))
		return 0;
	return 1;
}
