/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _FFPROC_H
#define _FFPROC_H

#include <listwise.h>

#include "gn.h"
#include "ff.h"

#include "map.h"
#include "strstack.h"
#include "path.h"

#define restrict __restrict

#define FFP_NOFILE			0x01	// $* is an absolute path (relative is default)

/// ffproc
//
// SUMMARY
//  process a fabfile
//
// PARAMETERS
//  ffp     - fabfile parser, for parsing fabfiles in invocations
//  pth     - path object to the fabfile
//  vmap    - variables map
//  stax    - listwise stax
//  staxa   - listwise stax
//  staxp   - offset to next free stax
//  [first] - set to the target of the first dependency found
//  flags   - FFP_NOFILE     : referenced with a nofile path - affects the value of $*
//
int ffproc(
    const ff_parser * const restrict ffp
	, const path * const restrict inpath
  , map * const restrict vmap
  , lstack *** const restrict stax
  , int * const restrict staxa
  , int * const restrict staxp
  , gn ** restrict first
	, const uint32_t flags
) __attribute__((nonnull(1,2,3,4,5)));

#undef restrict
#endif

