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
#include "xapi/errtab.h"
#include "xlinux.h"
#include "narrate.h"

#include "internal.h"
#include "category.internal.h"

#include "list.h"

#include "macros.h"
#include "search.h"

#define restrict __restrict


int category_name_max_length;

//
// constant
//

// maximum number of unique categories by name ; number of bits set in BITS_ID_OPT
#define MAX_CATEGORIES   48

// mask of bits in logger_category.bits containing the category id
#define BITS_ID_OPT      UINT64_C(0x0000FFFFFFFFFFFF)

// mask of bits in logger_category.bits containing the color option
#define BITS_COLOR_OPT   UINT64_C(0xFFFF000000000000)

// mask of bits in logger_category.attr containing the color resolution option
#define ATTR_COLOR_OPT   UINT32_C(0x1)

//
// static
//

// mask of all category ids which have already been used
static uint64_t used_category_ids_mask;

// registered but not yet resolved
static union {
  list;

  struct {
    logger_category **  v;
    size_t              l;
  };
} unresolved = {{ list_es : sizeof(logger_category *) }};

// resolved categories, sorted by name, rank
static union {
  list;

  struct {
    logger_category **  v;
    size_t              l;
  };
} resolved = {{ list_es : sizeof(logger_category *) }};

#define COMPARE_BYNAME 2
#define COMPARE_BYID   4
#define COMPARE_BYRANK 8

/// category_compare
//
// SUMMARY
//  sort categories
//
// PARAMETERS
//  keys - bitwise mask of COMPARE_* constants
// 
static int category_compare(const void * _A, const void * _B, void * _options)
{
  logger_category ** A = (void*)_A;
  logger_category ** B = (void*)_B;
  uint32_t options = (uint32_t)(uintptr_t)_options;

  int r = 0;
  if(r == 0 && (options & COMPARE_BYNAME))
    r = strcmp((*A)->name, (*B)->name);

  if(r == 0 && (options & COMPARE_BYID))
  {
    if(((*A)->bits & BITS_ID_OPT) > ((*B)->bits & BITS_ID_OPT))
      r = 1;
    else if(((*A)->bits & BITS_ID_OPT) < ((*B)->bits & BITS_ID_OPT))
      r = -1;
  }

  if(r == 0 && (options & COMPARE_BYRANK))
    r = (*A)->rank - (*B)->rank;

  return r;
}

/// next_category_id
//
// SUMMARY
//  retrieve the next unused category id bit
//
// PARAMETERS
//  bit - (returns) next unused bit
//
static uint64_t next_category_id(uint64_t * const restrict bit)
{
  enter;

  if(~used_category_ids_mask == 0)
    fail(LOGGER_TOOMANY);

  (*bit) = ~used_category_ids_mask & -~used_category_ids_mask;
  used_category_ids_mask |= (*bit);

finally:
  XAPI_INFOF("max", "%d", MAX_CATEGORIES);
coda;
}

/// bits_say
//
// SUMMARY
//  write a description of the options in bits to the narrator
//
xapi bits_say(uint64_t bits, narrator * _narrator)
{
  enter;

  if(bits & BITS_COLOR_OPT)
  {
    says(LOGGER_COLOR_STR(bits & BITS_COLOR_OPT));
  }

  finally : coda;
}

//
// api
//

API xapi logger_register(logger_category * logs)
{
  enter;

  while(logs->name)
  {
    fatal(list_append, &unresolved, &logs);
    logs++;
  }

  finally : coda;
}

xapi logger_register_resolve()
{
  enter;

  // copy from unresolved -> resolved
  fatal(list_append_range, &resolved, unresolved.v, unresolved.l);
  qsort_r(resolved.v, resolved.l, resolved.list_es, category_compare, (void*)(COMPARE_BYNAME | COMPARE_BYRANK));
  unresolved.l = 0;

  int x;
  int y;
  for(x = 0; x < resolved.l; x++)
  {
    // whether any category in the name-group has no id assigned
    int anyzero = !(resolved.v[x]->bits & BITS_ID_OPT);

    uint64_t id = resolved.v[x]->bits;

    // find the bounds of the name-group
    for(y = x + 1; y < resolved.l; y++)
    {
      if(category_compare(&resolved.v[x], &resolved.v[y], (void*)COMPARE_BYNAME))
        break;

      id |= resolved.v[y]->bits;
      anyzero |= !(resolved.v[y]->bits & BITS_ID_OPT);
    }

    // recalculate the bits if any member of the group was newly added
    if(anyzero)
    {
      id &= BITS_ID_OPT;
      size_t namel = strlen(resolved.v[x]->name);

      // assign the next unused bit
      if(!id)
      {
        fatal(next_category_id, &id);
        category_name_max_length = MAX(category_name_max_length, namel);
      }

      uint64_t options = 0;

      int i;
      for(i = x; i < y; i++)
      {
        int set = 0;

        // if the bits contain a non-default option, assume the value is to be set
        if(resolved.v[i]->bits & BITS_COLOR_OPT)
          set = 1;

        // the options may also explicitly specify this
        else if((resolved.v[i]->attr & ATTR_COLOR_OPT) == LOGGER_ATTR_COLOR_SET)
          set = 1;

        if(set)
        {
          options &= ~BITS_COLOR_OPT;
          options |= (resolved.v[i]->bits & BITS_COLOR_OPT);
        }

        resolved.v[i]->namel = namel;
      }

      for(i = x; i < y; i++)
        resolved.v[i]->bits = id | options;
    }

    x = y - 1;
  }

#if 0
  printf("resolved category bits\n");
  for(x = 0; x < resolved.l; x++)
  {
    // find the bounds of the name-group
    for(y = x + 1; y < resolved.l; y++)
    {
      if(category_compare(&resolved.v[x], &resolved.v[y], (void*)COMPARE_BYNAME))
        break;
    }

    char space[64];
    narrationw(space, sizeof(space));

    sayf("%*s : 0x%016lx ", category_name_max_length, resolved.v[x]->name, resolved.v[x]->bits);
    fatal(bits_say, resolved.v[x]->bits, _narrator);
    printf("%s\n", space);

    x = y - 1;
  }
#endif

  finally : coda;
}
