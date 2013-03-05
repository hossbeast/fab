#include <stddef.h>

#include "listwise/internal.h"

#include "coll.h"
#include "idx.h"

#include "control.h"

typedef char* charstar;

union object_registry_t object_registry = { { .size = sizeof(listwise_object*) } };

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
	return (int)(intptr_t)((*obj) = idx_lookup_val(object_registry.by_type, &type, 0));
}

///
/// [[ LSTACK API (for objects) ]]


void object_teardown()
{
	int x;
	for(x = 0; x < object_registry.l; x++)
		free(object_registry.e[x]->string_property);

	free(object_registry.e);

	idx_free(object_registry.by_type);
}
