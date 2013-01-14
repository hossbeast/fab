#ifndef _VAR_H
#define _VAR_H

#include <listwise.h>

#include "map.h"

int var_set(map * vmap, char * s, lstack * ls, int sticky)
	__attribute__((nonnull));

#endif
