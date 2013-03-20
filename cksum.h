#ifndef _CKSUM_H
#define _CKSUM_H

#include <sys/types.h>
#include <stdint.h>

#define restrict __restrict

/// cksum
//
// simple hash
//
uint32_t cksum(const void * const restrict v, size_t l);

#undef restrict
#endif
