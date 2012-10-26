#ifndef _XMEM_H
#define _XMEM_H

#include <stdlib.h>

/// xmalloc
//
// does this: *target = calloc(size, 1);
//  recall that calloc zeroes out the memory it allocates
//
// returns nonzero on success
//  
// example:
//
//  char* memory;
//  fatal(xmalloc, &memory, 10);
//
int xmalloc(void* target, size_t size) __attribute__((nonnull));

/// xrealloc
//
// does this: realloc(*target, size) AND ensures that any new portion of memory is
// zeroed out
//
// parameters
//  es  - element size
//  ec  - element count
//  oec - old element count
//
// returns
//   nonzero on success
//
// example:
//
// char* memory;
// fatal(xmalloc, &memory, 10, 0);
// ...
// fatal(xrealloc, &memory, 20, 10);
//
int xrealloc(void* target, size_t es, size_t ec, size_t oec) __attribute__((nonnull));

/// xfree
//
// does this:
//   free(*target);
//   *target = 0;
//
void xfree(void* target) __attribute__((nonnull));

#endif
