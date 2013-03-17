#ifndef _STRSTACK_H
#define _STRSTACK_H

#define restrict __restrict

/// opaque strstack type declaration
//
struct strstack_t;
typedef struct strstack_t strstack;

/// strstack_create
//
// create an empty strstack
//
// returns nonzero on success
//
int strstack_create(strstack ** const restrict stk)
  __attribute__((nonnull(1)));

/// strstack_free
//
// free a strstack with free semantics
//
void strstack_free(strstack * const restrict stk);

/// strstack_xfree
//
// free a strstack with xfree semantics
//
void strstack_xfree(strstack ** const restrict stk)
  __attribute__((nonnull));

/// strstack_undef
//
// remove all entries
//
// returns nonzero on success
//
void strstack_undef(strstack* const restrict stk)
  __attribute__((nonnull));

/// strstack_push
//
// push a string
//
// parameters
//  stk - strstack
//  v   - string
//
// returns nonzero on success
//
int strstack_push(strstack * const restrict stk, const void* const restrict v)
  __attribute__((nonnull));

/// strstack_pop
//
// pop
//
// returns pointer to the stored value, or 0 if not found
//
void strstack_pop(strstack * const restrict stk)
  __attribute__((nonnull));

/// strstack_string
//
// get a string representing stack contents
//
// PARAMETERS
//  stk   - strstack instance
//  [ldr] - prepended to the result string
//  [d]   - delimite stk members in the result string
//  r     - results returned here - points to memory in the strstack instance
//
// returns nonzero on success
//
int strstack_string(strstack * const restrict stk, const char * const restrict ldr, const char * const restrict d, char ** const restrict r)
  __attribute__((nonnull(1, 4)));

#endif
