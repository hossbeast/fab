#ifndef _FFPROC_H
#define _FFPROC_H

#include <listwise.h>

#include "gn.h"
#include "ff.h"

#include "map.h"
#include "strstack.h"
#include "path.h"

#define restrict __restrict

#define FFP_NOFILE			0x01	// $^ is an absolute path (relative is default)
#define FFP_EXACTPATH		0x04	// can be invoked multiple times (single invocation is default)

/// ffproc
//
// SUMMARY
//  process a fabfile
//
// PARAMETERS
//  ffp     - fabfile parser, for parsing fabfiles in invocations
//  pth     - path object to the fabfile
//  sstk    - scope stack
//  vmap    - variables map
//  stax    - listwise stax
//  staxa   - listwise stax
//  staxp   - offset to next free stax
//  [first] - set to the target of the first dependency found
//  flags   - FFP_NOFILE     : referenced with a nofile path - affects the value of $^
//            FFP_EXACTPATH  : referenced with an exact path - can be invoked multiple times
//
int ffproc(
    const ff_parser * const restrict ffp
	, const path * const restrict inpath
  , strstack * const restrict sstk
  , map * const restrict vmap
  , lstack *** const restrict stax
  , int * const restrict staxa
  , int * const restrict staxp
  , gn ** restrict first
	, const uint32_t flags
) __attribute__((nonnull(1,2,3,4,5,6)));

#undef restrict
#endif
