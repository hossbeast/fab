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

#include "xapi.h"
#include "types.h"

#include "attr.h"

struct narrator;

#define COLOR_OPT     UINT32_C(0x0000000F)
#define CATEGORY_OPT  UINT32_C(0x00000030)
#define TRACE_OPT     UINT32_C(0x000000C0)
#define DISCOVERY_OPT UINT32_C(0x00000300)
#define DATESTAMP_OPT UINT32_C(0x00000C00)
#define NAMES_OPT     UINT32_C(0x00003000)
#define FILTER_OPT    UINT32_C(0x0000C000)
#define PID_OPT       UINT32_C(0x00030000)

const char * color_option_name(uint32_t attrs);
const char * category_option_name(uint32_t attrs);
const char * trace_option_name(uint32_t attrs);
const char * discovery_option_name(uint32_t attrs);
const char * datestamp_option_name(uint32_t attrs);
const char * processid_option_name(uint32_t attrs);
const char * filter_option_name(uint32_t attrs);
const char * pid_option_name(uint32_t attrs);

/// attr_combine
//
// SUMMARY
//  combine two sets of attributes
//
// PARAMETERS
//  A - lower precedence
//  B - higher precedence
//
// RETURNS
//  attribute set A overwritten with all options affirmatively set by B
//
uint32_t attr_combine2(uint32_t A, uint32_t B);
uint32_t attr_combine4(uint32_t A, uint32_t B, uint32_t C, uint32_t D);

/// attr_say
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

uint32_t attr_byname(const char * name, size_t namel)
  __attribute__((nonnull));

#endif
