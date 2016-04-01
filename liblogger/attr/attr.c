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

#include "xapi.h"
#include "narrator.h"

#include "internal.h"
#include "attr/attr.internal.h"

#define restrict __restrict

uint32_t attr_combine(uint32_t A, uint32_t B)
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

  return A;
}

xapi attr_say(uint32_t attr, narrator * const restrict N)
{
  enter;

  int wrote = 0;

  if(attr & COLOR_OPT)
  {
    if(wrote)
      sayc(',');
    wrote = 1;

    says(COLOR_VALUE(attr));
  }

  if(attr & CATEGORY_OPT)
  {
    if(wrote)
      sayc(',');
    wrote = 1;

    says(CATEGORY_VALUE(attr));
  }

  if(attr & TRACE_OPT)
  {
    if(wrote)
      sayc(',');
    wrote = 1;

    says(TRACE_VALUE(attr));
  }

  if(attr & DISCOVERY_OPT)
  {
    if(wrote)
      sayc(',');
    wrote = 1;

    says(DISCOVERY_VALUE(attr));
  }

  if(attr & TIMESTAMP_OPT)
  {
    if(wrote)
      sayc(',');
    wrote = 1;

    says(TIMESTAMP_VALUE(attr));
  }

  finally : coda;
}
