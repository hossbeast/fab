#ifndef _CKSUM_H
#define _CKSUM_H

#include <sys/types.h>
#include <stdint.h>

#define restrict __restrict

/// cksum
//
// simple hash
//
uint32_t cksum(const char * const restrict s, size_t l);

#undef restrict
#endif
