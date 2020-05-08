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

#include <string.h>
#include <stdlib.h>

#include "xapi.h"
#include "types.h"

#include "narrator.h"
#include "valyria/array.h"

#include "internal.h"
#include "opts.internal.h"
#include "stream.internal.h"

#include "attrs.h"
#include "macros.h"

#define LOGGER_ATTRS_TABLES  \
  LOGGER_COLOR_OPT_TABLE     \
  LOGGER_CATEGORY_OPT_TABLE  \
  LOGGER_TRACE_OPT_TABLE     \
  LOGGER_DISCOVERY_OPT_TABLE \
  LOGGER_DATESTAMP_OPT_TABLE \
  LOGGER_NAMES_OPT_TABLE     \
  LOGGER_FILTER_OPT_TABLE    \
  LOGGER_PID_OPT_TABLE       \
  LOGGER_TID_OPT_TABLE       \

static attrs32 logger_attrs = (attrs32){
#define LOGGER_ATTR_DEF(x, y, r) + 1
    num : 0 LOGGER_ATTRS_TABLES
  , members : (member32[]) {
#undef LOGGER_ATTR_DEF
#define LOGGER_ATTR_DEF(x, y, r) { name : #x, value : UINT32_C(y), range : UINT32_C(r) },
LOGGER_ATTRS_TABLES
  }
};

static void __attribute__((constructor)) init()
{
  attrs32_init(&logger_attrs);
}

const char * color_option_name(uint32_t attrs)     { return attrs32_name_byvalue(&logger_attrs, LOGGER_COLOR_OPT & attrs); }
const char * category_option_name(uint32_t attrs)  { return attrs32_name_byvalue(&logger_attrs, LOGGER_CATEGORY_OPT & attrs); }
const char * trace_option_name(uint32_t attrs)     { return attrs32_name_byvalue(&logger_attrs, LOGGER_TRACE_OPT & attrs); }
const char * discovery_option_name(uint32_t attrs) { return attrs32_name_byvalue(&logger_attrs, LOGGER_DISCOVERY_OPT & attrs); }
const char * datestamp_option_name(uint32_t attrs) { return attrs32_name_byvalue(&logger_attrs, LOGGER_DATESTAMP_OPT & attrs); }
const char * processid_option_name(uint32_t attrs) { return attrs32_name_byvalue(&logger_attrs, LOGGER_NAMES_OPT & attrs); }
const char * filter_option_name(uint32_t attrs)    { return attrs32_name_byvalue(&logger_attrs, LOGGER_FILTER_OPT & attrs); }
const char * pid_option_name(uint32_t attrs)       { return attrs32_name_byvalue(&logger_attrs, LOGGER_PID_OPT & attrs); }
const char * tid_option_name(uint32_t attrs)       { return attrs32_name_byvalue(&logger_attrs, LOGGER_TID_OPT & attrs); }

uint32_t attrs_value(const char * restrict name, uint8_t namel)
{
  return attrs32_value_bynamew(&logger_attrs, name, namel);
}

//
// public
//

uint32_t attrs_combine2(uint32_t A, uint32_t B)
{
  if(B & LOGGER_COLOR_OPT)
  {
    A &= ~LOGGER_COLOR_OPT;
    A |= (B & LOGGER_COLOR_OPT);
  }

  if(B & LOGGER_CATEGORY_OPT)
  {
    A &= ~LOGGER_CATEGORY_OPT;
    A |= (B & LOGGER_CATEGORY_OPT);
  }

  if(B & LOGGER_TRACE_OPT)
  {
    A &= ~LOGGER_TRACE_OPT;
    A |= (B & LOGGER_TRACE_OPT);
  }

  if(B & LOGGER_DISCOVERY_OPT)
  {
    A &= ~LOGGER_DISCOVERY_OPT;
    A |= (B & LOGGER_DISCOVERY_OPT);
  }

  if(B & LOGGER_DATESTAMP_OPT)
  {
    A &= ~LOGGER_DATESTAMP_OPT;
    A |= (B & LOGGER_DATESTAMP_OPT);
  }

  if(B & LOGGER_NAMES_OPT)
  {
    A &= ~LOGGER_NAMES_OPT;
    A |= (B & LOGGER_NAMES_OPT);
  }

  if(B & LOGGER_FILTER_OPT)
  {
    A &= ~LOGGER_FILTER_OPT;
    A |= (B & LOGGER_FILTER_OPT);
  }

  if(B & LOGGER_PID_OPT)
  {
    A &= ~LOGGER_PID_OPT;
    A |= (B & LOGGER_PID_OPT);
  }

  if(B & LOGGER_TID_OPT)
  {
    A &= ~LOGGER_TID_OPT;
    A |= (B & LOGGER_TID_OPT);
  }

  return A;
}

uint32_t attrs_combine4(uint32_t A, uint32_t B, uint32_t C, uint32_t D)
{
  return attrs_combine2(attrs_combine2(attrs_combine2(A, B), C), D);
}

xapi attrs_say(uint32_t attrs, narrator * const restrict N)
{
  enter;

  char buf[256];
  size_t z = 0;

  z += znload_attrs32(buf, sizeof(buf), &logger_attrs, attrs);

  xsayw(buf, z);

  finally : coda;
}
