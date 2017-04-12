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

#include <stdio.h>
#include <inttypes.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/pstring.h"
#include "valyria/list.h"
#include "valyria/dictionary.h"
#include "valyria/map.h"

#include "internal.h"
#include "category.internal.h"
#include "LOGGER.errtab.h"
#include "log.internal.h"
#include "attr.internal.h"
#include "logging.internal.h"

#include "macros.h"


/// location
//
// tracks two sequences of elements with the same name that are common to both lists
//
typedef struct location {
  int ax;     // index of the first element in A
  int axl;    // number of elements in A
  int bx;     // index of the first element in B
  int bxl;    // number of elements in B
} location;

#define restrict __restrict

int category_name_max_length;
uint64_t category_optional_mask;

// maximum number of unique categories by name
#define MAX_CATEGORIES  (sizeof(((logger_category*)0)->id) * 8)

//
// static
//

// categories that have been registered but not yet activated
static list * registered;

// working space for category_list_merge
static list * registering;

// activated categories, in decreasing order of precedence
list * activated;

static map * activated_byname;
static map * activated_byid;

//
// public
//

xapi category_setup()
{
  enter;

  fatal(list_create, &registered);
  fatal(list_create, &registering);
  fatal(list_create, &activated);
  fatal(map_create, &activated_byname);
  fatal(map_create, &activated_byid);

  finally : coda;
}

xapi category_cleanup()
{
  enter;

  fatal(list_ixfree, &registered);
  fatal(list_ixfree, &registering);
  fatal(list_ixfree, &activated);
  fatal(map_ixfree, &activated_byname);
  fatal(map_ixfree, &activated_byid);

  finally : coda;
}

xapi category_say_verbose(logger_category * const restrict cat, narrator * restrict N)
{
  enter;

  sayf("%s%*s : 0x%016"PRIx64 " 0x%08"PRIx32 " : "
    , cat->optional ? "*" : " "
    , category_name_max_length
    , cat->name
    , cat->id
    , cat->attr
  );

  says(cat->description);
  if(cat->attr)
  {
    says(", %");
    fatal(attr_say, cat->attr, N);
  }

  finally : coda;
}

xapi category_say(logger_category * const restrict cat, narrator * restrict N)
{
  enter;

  sayf("%s%*s : %s"
    , cat->optional ? "*" : " "
    , category_name_max_length
    , cat->name
    , cat->description
  );

  finally : coda;
}

uint32_t categories_attrs(uint64_t ids)
{
  uint32_t attrs = 0;

  // categories
  uint64_t copy = ids;
  uint64_t bit = UINT64_C(1) << 63;
  while(copy)
  {
    if(copy & bit)
    {
      logger_category * category = category_byid(bit);
      if(category)
        attrs = attr_combine2(attrs, category->attr);

      copy &= ~bit;
    }
    bit >>= 1;
  }

  return attrs;
}

xapi category_list_merge(list * restrict A, list * restrict B, list * restrict C)
{
  enter;

  dictionary * common = 0;
  location ** ax = 0;
  location ** bx = 0;
  int c = 0;    // number of sequences in common
  int x;

  // build a map of element sequences common between the two lists
  fatal(dictionary_create, &common, sizeof(location));

  x = 0;
  int y = 0;
  while(x < list_size(A) || y < list_size(B))
  {
    if(x < list_size(A))
    {
      logger_category * a = list_get(A, x);
      location * loc;
      if((loc = dictionary_get(common, MMS(a->name))))
      {
        if(loc->ax != -1 && (loc->ax + loc->axl) != x)
        {
          failf(LOGGER_ILLREPEAT, "category", "%s", a->name);
        }
      }
      else
      {
        fatal(dictionary_set, common, MMS(a->name), &loc);
        loc->ax = -1;
        loc->bx = -1;
      }

      if(loc->ax == -1)
        loc->ax = x;
      loc->axl++;
      x++;
    }

    if(y < list_size(B))
    {
      logger_category * b = list_get(B, y);
      location * loc;
      if((loc = dictionary_get(common, MMS(b->name))))
      {
        if(loc->bx != -1 && (loc->bx + loc->bxl) != y)
        {
          failf(LOGGER_ILLREPEAT, "category", "%s", b->name);
        }
      }
      else
      {
        fatal(dictionary_set, common, MMS(b->name), &loc);
        loc->ax = -1;
        loc->bx = -1;
      }
      if(loc->bx == -1)
        loc->bx = y;
      loc->bxl++;
      y++;
    }
  }

  // create a copy of the common sequence locations for each list
  fatal(xmalloc, &ax, sizeof(*ax) * dictionary_size(common));
  fatal(xmalloc, &bx, sizeof(*bx) * dictionary_size(common));

  for(x = 0; x < dictionary_table_size(common); x++)
  {
    location * loc = 0;
    if((loc = dictionary_table_value(common, x)))
    {
      if(loc->ax >= 0 && loc->bx >= 0)
      {
        ax[c] = loc;
        bx[c] = loc;
        c++;
      }
    }
  }

  // sort the per-list sequence lists
  int compar(const void * _A, const void * _B, void * arg)
  {
    location * A = *(location **)_A;
    location * B = *(location **)_B;

    if(arg)
      return A->ax - B->ax;
    else
      return A->bx - B->bx;
  };
  qsort_r(ax, c, sizeof(*ax), compar, (void*)1);
  qsort_r(bx, c, sizeof(*bx), compar, (void*)0);

  // walk the lists, write to C, and verify that the common elements appear in the same order
  fatal(list_recycle, C);

  x = 0;
  y = 0;
  int cx = 0;
  while(x < list_size(A) || y < list_size(B))
  {
    logger_category * a = 0;
    logger_category * b = 0;

    if(cx < c && x == ax[cx]->ax && y == bx[cx]->bx)
    {
      a = list_get(A, x);
      b = list_get(B, y);

      // common elements must appear in the same order
      if(strcmp(a->name, b->name))
      {
        xapi_info_pushf("categories", "%s, %s", a->name, b->name);
        fail(LOGGER_ILLORDER);
      }

      // take all elements from both sequences
      fatal(list_replicate, C, C->l, A, x, ax[cx]->axl);
      x += ax[cx]->axl;
      fatal(list_replicate, C, C->l, B, y, bx[cx]->bxl);
      y += bx[cx]->bxl;

      cx++;
    }
    else
    {
      if(x < list_size(A) && (cx == c || x < ax[cx]->ax))
      {
        a = list_get(A, x);
        location * loc = dictionary_get(common, MMS(a->name));
        fatal(list_replicate, C, C->l, A, x, loc->axl);
        x += loc->axl;
      }

      else // if(y < list_size(B))
      {
        b = list_get(B, y);
        location * loc = dictionary_get(common, MMS(b->name));
        fatal(list_replicate, C, C->l, B, y, loc->bxl);
        y += loc->bxl;
      }
    }
  }

finally:
  fatal(dictionary_xfree, common);
  wfree(ax);
  wfree(bx);
coda;
}

//
// api
//

API xapi categories_report_verbose()
{
  enter;

  logs(L_LOGGER, "logger categories");

  int x;
  for(x = 0; x < list_size(activated); x++)
  {
    // find the bounds of the name-group
    int y;
    for(y = x + 1; y < list_size(activated); y++)
    {
      logger_category * A = list_get(activated, x);
      logger_category * B = list_get(activated, y);
      if(strcmp(A->name, B->name))
        break;
    }

    narrator * N;
    fatal(log_start, L_LOGGER, &N);
    says(" ");
    fatal(category_say_verbose, list_get(activated, x), N);
    fatal(log_finish);

    x = y - 1;
  }

  finally : coda;
}

API xapi categories_activate()
{
  enter;

  category_name_max_length = 8;
  category_optional_mask = 0;

  // mask of category ids that have been assigned
  uint64_t used_category_ids_mask = 0;

  list * activating = 0;
  map * activating_byname = 0;
  map * activating_byid = 0;
  list * sublist = 0;

  fatal(list_create, &activating);
  fatal(map_create, &activating_byname);
  fatal(map_create, &activating_byid);

  // merge the activated list with the registered list
  fatal(category_list_merge, registered, activated, activating);

  int x;
  for(x = 0; x < list_size(activating); x++)
  {
    // whether any category in the name-group has no id assigned
    int anyzero = !((logger_category *)list_get(activating, x))->id;

    // find the bounds of the name-group
    int y;
    for(y = x + 1; y < list_size(activating); y++)
    {
      logger_category * A = list_get(activating, x);
      logger_category * B = list_get(activating, y);
      if(strcmp(A->name, B->name))
        break;

      anyzero |= !B->id;
    }

    // assign the id as the next unused bit from the mask
    if(~used_category_ids_mask == 0)
      failf(LOGGER_TOOMANY, "limit", "%lu", MAX_CATEGORIES);

    uint64_t id = ~used_category_ids_mask & -~used_category_ids_mask;
    used_category_ids_mask |= id;

    // assign the id to all members of the name-group
    int i;
    for(i = x; i < y; i++)
      ((logger_category *)list_get(activating, i))->id = id;

    logger_category * category = list_get(activating, x);
    char * name = category->name;
    size_t namel = strlen(name);
    category_name_max_length = MAX(category_name_max_length, namel);

    // recalculate name-group attributes if any member was newly added
    if(anyzero)
    {
      // in rank order
      fatal(list_sublist, activating, x, y - x, &sublist);

      int compar(const void * A, const void * B, void * arg)
      {
        return ((const logger_category *)A)->rank - ((const logger_category *)B)->rank;
      }
      list_sort(sublist, compar, 0);

      uint32_t attr = 0;
      char optional = 0;
      for(i = 0; i < list_size(sublist); i++)
      {
        logger_category * cat = list_get(sublist, i);
        attr = attr_combine2(attr, cat->attr);
        if(i == 0)
          optional = cat->optional;
      }

      for(i = 0; i < list_size(sublist); i++)
      {
        logger_category * A = list_get(sublist, i);
        A->namel = namel;
        A->attr = attr;
        A->optional = optional;
      }
    }

    if(category->optional)
      category_optional_mask |= category->id;

    // assign to lookup map
    fatal(map_set, activating_byname, name, namel, category);
    fatal(map_set, activating_byid, MM(id), category);

    x = y - 1;
  }

  // swap activating and activated
  void * T = activated;
  activated = activating;
  activating = T;

  T = activated_byname;
  activated_byname = activating_byname;
  activating_byname = T;

  T = activated_byid;
  activated_byid = activating_byid;
  activating_byid = T;

  fatal(list_ixfree, &registered);
  fatal(list_ixfree, &registering);

finally:
  fatal(list_xfree, activating);
  fatal(map_xfree, activating_byname);
  fatal(map_xfree, activating_byid);
  fatal(list_xfree, sublist);
coda;
}

API xapi logger_categories_report()
{
  enter;

  narrator * N;

  int x;
  for(x = 0; x < list_size(activated); x++)
  {
    // find the bounds of the name-group
    int y;
    for(y = x + 1; y < list_size(activated); y++)
    {
      logger_category * A = list_get(activated, x);
      logger_category * B = list_get(activated, y);
      if(strcmp(A->name, B->name))
        break;
    }

    fatal(log_start, L_LOGGER, &N);
    says(" ");
    fatal(category_say, list_get(activated, x), N);
    fatal(log_finish);

    x = y - 1;
  }

  finally : coda;
}

API xapi logger_category_register(logger_category * logs)
{
  enter;

  if(!registered)
    fatal(list_create, &registered);

  if(!registering)
    fatal(list_create, &registering);

  list * tmp = 0;
  fatal(list_create, &tmp);
  while(logs->name)
  {
    fatal(list_push, tmp, logs);
    logs++;
  }

  // merge the registered list with the new list
  fatal(category_list_merge, tmp, registered, registering);

  // swap registering and registered
  void * T = registered;
  registered = registering;
  registering = T;

finally:
  fatal(list_xfree, tmp);
coda;
}

API logger_category * category_byname(const char * const restrict name, size_t namel)
{
  namel = namel ?: strlen(name);
  return map_get(activated_byname, name, namel);
}

API logger_category * category_byid(uint64_t id)
{
  return map_get(activated_byid, MM(id));
}
