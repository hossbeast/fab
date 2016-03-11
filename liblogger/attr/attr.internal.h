/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LOGGER_ATTR_INTERNAL_H
#define _LOGGER_ATTR_INTERNAL_H

#include <inttypes.h>

#include "xapi.h"

#include "attr.h"

struct narrator;

#define COLOR_OPT     UINT32_C(0x0000000F)
#define CATEGORY_OPT  UINT32_C(0x00000030)
#define TRACE_OPT     UINT32_C(0x000000C0)
#define DISCOVERY_OPT UINT32_C(0x00000300)
#define TIMESTAMP_OPT UINT32_C(0x00000C00)

// get the name of a color from its constant
#define LOGGER_ATTR_DEF(a, b, x, m) (x & m) == b ? "L_" #a : 
#define COLOR_VALUE(x)      LOGGER_ATTR_TABLE(x, COLOR_OPT) "NONE"
#define CATEGORY_VALUE(x)   LOGGER_ATTR_TABLE(x, CATEGORY_OPT) "NONE"
#define TRACE_VALUE(x)      LOGGER_ATTR_TABLE(x, TRACE_OPT) "NONE"
#define DISCOVERY_VALUE(x)  LOGGER_ATTR_TABLE(x, DISCOVERY_OPT) "NONE"
#define TIMESTAMP_VALUE(x)  LOGGER_ATTR_TABLE(x, TIMESTAMP_OPT) "NONE"

#define restrict __restrict

/// attr_combine
//
// SUMMARY
//  combine two sets of attributes
//
// PARAMETERS
//  A - low precedence
//  B - high precedence
//  
// RETURNS
//  attribute set A overwritten with all options affirmatively set by B
//
uint32_t attr_combine(uint32_t A, uint32_t B);

/// category_attr_say
//
// SUMMARY
//  write a description of attr to the narrator
//
// PARAMETERS
//  attr     - 
//  narrator - 
//
xapi attr_say(uint32_t attr, struct narrator * const restrict _narrator)
  __attribute__((nonnull));

#undef restrict
#endif
