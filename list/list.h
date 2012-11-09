#ifndef _LIST_H
#define _LIST_H

#include <listwise.h>
#include <listwise/generator.h>

#include "ff.h"

/// list_render
//
// render the list node to the lstack
//
list_render(ff_node * list, generator_parser ** p, lstack ** ls);

#endif
