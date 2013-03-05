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

int op_load(char* path);
void op_sort();
void op_teardown();


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
