#ifndef _FFPROC_H
#define _FFPROC_H

#include <listwise.h>

#include "gn.h"
#include "ff.h"

#include "map.h"
#include "strstack.h"
#include "path.h"

#define restrict __restrict

#define FFP_MODULE	0x01

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
//  flags   - FFP_MODULE : this ff was referenced with module notation - affects the value of $^
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
