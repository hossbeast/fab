#ifndef _MACROS_H
#define _MACROS_H

#include <listwise.h>
#include <listwise/lstack.h>

#define ITERATE(x, y, z)			\
	if((x))											\
	{														\
		if((x)->l)								\
		{													\
			LSTACK_ITERATE(x, y, z)

#define ITEREND } } LSTACK_ITEREND

#define MIN(a,b)            \
 ({ typeof (a) _a = (a);    \
     typeof (b) _b = (b);   \
   _a > _b ? _b : _a; })

#define MAX(a,b)            \
 ({ typeof (a) _a = (a);    \
     typeof (b) _b = (b);   \
   _a > _b ? _a : _b; })

#endif
