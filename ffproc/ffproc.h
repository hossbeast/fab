#ifndef _FFPROC_H
#define _FFPROC_H

#include <listwise.h>

#include "gn.h"
#include "ff.h"
#include "map.h"
#include "strstack.h"

#define restrict __restrict

/// ffproc
//
// SUMMARY
//  process a fabfile
//
// PARAMETERS
//  ffn     - root node of the parsed fabfile
//  ffp     - fabfile parser, for parsing fabfiles in invocations
//  dir     - relative path to the directory containing the fabfile
//  sstk    - scope stack
//  vmap    - variables map
//  stax    - listwise stax
//  staxa   - listwise stax
//  staxp   - offset to next free stax
//  [first] - set to first dependency processed
//
int ffproc(
    const ff_node * const restrict ffn
  , const ff_parser * const restrict ffp
  , const char * const restrict dir
  , strstack * const restrict sstk
  , map * const restrict vmap
  , lstack *** const restrict stax
  , int * const restrict staxa
  , int staxp
  , gn ** const restrict first
) __attribute__((nonnull(1,2,3,4,5,6,7)));

#endif
