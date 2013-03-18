#ifndef _ENCLOSE_H
#define _ENCLOSE_H

#include "ffn.h"

#define restrict __restrict

/// enclose_vars
//
// SUMMARY
//
// PARAMETERS
//
// RETURNS
//
int enclose_vars(ff_node * const restrict root, ff_node *** restrict closure_vars, int * const restrict closure_varsa, int * const restrict closure_varsl)
	__attribute__((nonnull));

#undef restrict
#endif
