#ifndef _LIST_H
#define _LIST_H

#include <listwise.h>

#include "ff.h"

/// list_render
//
// render the list node to the lstack
//
int list_render(ff_node * list, lstack *** stax, int * lstax_l, int * stax_a, int p);

#endif
