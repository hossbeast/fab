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

#include <stddef.h>

#include "internal.h"

#include "coll.h"
#include "map.h"

//
// public
//

void object_teardown()
{
	map_free(object_registry);
}

///
/// [[ LSTACK API (for objects) ]]
///

API xapi listwise_register_object(uint8_t type, listwise_object * def)
{
	enter;

	def->type = type;

	if(object_registry == 0)
		fatal(map_create, &object_registry, 0);

	// cache for lookup
	fatal(map_set, object_registry, MM(type), MM(def), 0);

	finally : coda;
}

API xapi listwise_enumerate_objects(listwise_object *** list, size_t * list_len)
{
	xproxy(map_values, object_registry, list, list_len);
}

API xapi listwise_lookup_object(uint8_t type, listwise_object ** obj)
{
	enter;

	listwise_object ** rv = 0;
	if((rv = map_get(object_registry, MM(type))) == 0)
	{
		fail(LW_NOOBJ);
	}

	(*obj) = (*rv);

finally:
	XAPI_INFOF("type", "%hhu", type);
coda;
}
