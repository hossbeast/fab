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
#include "narrator.h"
#include "valyria/array.h"

#include "internal.h"
#include "attr.internal.h"
#include "stream.internal.h"

#include "attrs32.h"
#include "macros.h"

#define restrict __restrict

struct attrs32 * logger_attrs = (struct attrs32[]) {
#define LOGGER_ATTR_DEF(a, b) { value : (b), name : #a, namel : strlen(#a) },
LOGGER_ATTR_TABLE
#undef LOGGER_ATTR_DEF
};

#define LOGGER_ATTR_DEF(a, b) + 1
size_t logger_attrs_len = 0 LOGGER_ATTR_TABLE;
#undef LOGGER_ATTR_DEF

static void __attribute__((constructor)) attr_init()
{
  qsort(logger_attrs, logger_attrs_len, sizeof(*logger_attrs), attrs32_compare_byname);
}

uint32_t attr_byname(const char * name, size_t namel)
{
  return attrs32_lookup_byname(logger_attrs, logger_attrs_len, name, namel);
}

const char * color_option_name(uint32_t attrs)     { return attrs32_option_name(logger_attrs, logger_attrs_len, COLOR_OPT, attrs); }
const char * category_option_name(uint32_t attrs)  { return attrs32_option_name(logger_attrs, logger_attrs_len, CATEGORY_OPT, attrs); }
const char * trace_option_name(uint32_t attrs)     { return attrs32_option_name(logger_attrs, logger_attrs_len, TRACE_OPT, attrs); }
const char * discovery_option_name(uint32_t attrs) { return attrs32_option_name(logger_attrs, logger_attrs_len, DISCOVERY_OPT, attrs); }
const char * datestamp_option_name(uint32_t attrs) { return attrs32_option_name(logger_attrs, logger_attrs_len, DATESTAMP_OPT, attrs); }
const char * processid_option_name(uint32_t attrs) { return attrs32_option_name(logger_attrs, logger_attrs_len, NAMES_OPT, attrs); }
const char * filter_option_name(uint32_t attrs)    { return attrs32_option_name(logger_attrs, logger_attrs_len, FILTER_OPT, attrs); }

//
// public
//

uint32_t attr_combine2(uint32_t A, uint32_t B)
{
  if(B & COLOR_OPT)
  {
    A &= ~COLOR_OPT;
    A |= (B & COLOR_OPT);
  }

  if(B & CATEGORY_OPT)
  {
    A &= ~CATEGORY_OPT;
    A |= (B & CATEGORY_OPT);
  }

  if(B & TRACE_OPT)
  {
    A &= ~TRACE_OPT;
    A |= (B & TRACE_OPT);
  }

  if(B & DISCOVERY_OPT)
  {
    A &= ~DISCOVERY_OPT;
    A |= (B & DISCOVERY_OPT);
  }

  if(B & DATESTAMP_OPT)
  {
    A &= ~DATESTAMP_OPT;
    A |= (B & DATESTAMP_OPT);
  }

  if(B & NAMES_OPT)
  {
    A &= ~NAMES_OPT;
    A |= (B & NAMES_OPT);
  }

  if(B & FILTER_OPT)
  {
    A &= ~FILTER_OPT;
    A |= (B & FILTER_OPT);
  }

  return A;
}

uint32_t attr_combine4(uint32_t A, uint32_t B, uint32_t C, uint32_t D)
{
  return attr_combine2(attr_combine2(attr_combine2(A, B), C), D);
}

xapi attr_say(uint32_t attr, narrator * const restrict N)
{
  enter;

  int wrote = 0;

  if(attr & COLOR_OPT)
  {
    if(wrote)
      xsayc(',');
    wrote = 1;

    xsays(color_option_name(attr));
  }

  if(attr & CATEGORY_OPT)
  {
    if(wrote)
      xsayc(',');
    wrote = 1;

    xsays(category_option_name(attr));
  }

  if(attr & TRACE_OPT)
  {
    if(wrote)
      xsayc(',');
    wrote = 1;

    xsays(trace_option_name(attr));
  }

  if(attr & DISCOVERY_OPT)
  {
    if(wrote)
      xsayc(',');
    wrote = 1;

    xsays(discovery_option_name(attr));
  }

  if(attr & DATESTAMP_OPT)
  {
    if(wrote)
      xsayc(',');
    wrote = 1;

    xsays(datestamp_option_name(attr));
  }

  if(attr & NAMES_OPT)
  {
    if(wrote)
      xsayc(',');
    wrote = 1;

    xsays(processid_option_name(attr));
  }

  if(attr & FILTER_OPT)
  {
    if(wrote)
      xsayc(',');
    wrote = 1;

    xsays(filter_option_name(attr));
  }

  finally : coda;
}
