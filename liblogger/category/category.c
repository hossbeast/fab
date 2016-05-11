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
#include "xlinux.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/pstring.h"

#include "internal.h"
#include "errtab/LOGGER.errtab.h"
#include "category/category.internal.h"
#include "log/log.internal.h"
#include "attr/attr.internal.h"
#include "logging.internal.h"

#define LIST_ELEMENT_TYPE logger_category
#include "valyria/list.h"

/// location
//
// tracks two sequences of elements with the same name that are common to both lists
//
struct location {
  int ax;     // index of the first element in A
  int axl;    // number of elements in A
  int bx;     // index of the first element in B
  int bxl;    // number of elements in B
};

typedef struct location location;
#define DICTIONARY_VALUE_TYPE struct location
#include "valyria/dictionary.h"

#include "valyria/map.h"
#include "macros.h"

#define restrict __restrict

int category_name_max_length;

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
static list * activated;

static map * activated_byname;
static map * activated_byid;

/// category_list_merge
//
// SUMMARY
//  merge two lists of categories in such a way that preserves the ordering of
//  each element relative to other elements in its source list
//
// PARAMETERS
//  A - source list
//  B - source list
//  C - dest list
//
// THROWS
//  ILLORDER - relative ordering of all elements cannot be maintained
//
static xapi __attribute__((nonnull)) category_list_merge(list * const restrict A, list * const restrict B, list *  const restrict C)
{
  enter;

  dictionary * common = 0;
  location ** ax = 0;
  location ** bx = 0;

  // build a map of element sequences common between the two lists
  fatal(dictionary_create, &common, sizeof(location));

  int x = 0;
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

  int c = 0;
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
  list_clear(C);

  x = 0;
  y = 0;
  int cx = 0;
  while(x < list_size(A) || y < list_size(B))
  {
    logger_category * a = 0;
    if(x < list_size(A))
      a = list_get(A, x);

    logger_category * b = 0;
    if(y < list_size(B))
      b = list_get(B, y);

    if(cx < c && x == ax[cx]->ax && y == bx[cx]->bx)
    {
      // common elements must appear in the same order
      if(strcmp(a->name, b->name))
      {
        xapi_fail_intent();
        xapi_infof("category A", "%s", a->name);
        xapi_infof("category B", "%s", b->name);
        fail(LOGGER_ILLORDER);
      }

      // take all elements from both sequences
      int i;
      for(i = 0; i < ax[cx]->axl; i++)
      {
        a = list_get(A, x);
        fatal(list_push, C, a);
        x++;
      }
      for(i = 0; i < bx[cx]->bxl; i++)
      {
        b = list_get(B, y);
        fatal(list_push, C, b);
        y++;
      }

      cx++;
    }
    else
    {
      if((cx == c || x < ax[cx]->ax) && a)
      {
        fatal(list_push, C, a);
        x++;
      }

      if((cx == c || y < bx[cx]->bx) && b)
      {
        fatal(list_push, C, b);
        y++;
      }
    }
  }

finally:
  dictionary_free(common);
  free(ax);
  free(bx);
coda;
}

//
// public
//

xapi category_say_verbose(logger_category * const restrict cat, narrator * restrict N)
{
  enter;

  sayf("%*s : 0x%016"PRIx64 " 0x%08"PRIx32 " : "
    , category_name_max_length
    , cat->name
    , cat->id
    , cat->attr
  );

  fatal(attr_say, cat->attr, N);
  says(cat->description);
  if(cat->attr)
  {
    says(", +");
    fatal(attr_say, cat->attr, N);
  }

  finally : coda;
}

API xapi category_report_verbose()
{
  enter;

  int token = 0;
  logs(L_LOGGER, "logger categories");

  int x;
  for(x = 0; x < list_size(activated); x++)
  {
    // find the bounds of the name-group
    int y;
    for(y = x + 1; y < list_size(activated); y++)
    {
      if(strcmp(list_get(activated, x)->name, list_get(activated, y)->name))
        break;
    }

    fatal(log_start, L_LOGGER, &token);
    narrator * N = log_narrator();
    says(" ");
    fatal(category_say_verbose, list_get(activated, x), N);
    fatal(log_finish, &token);

    x = y - 1;
  }

finally:
  fatal(log_finish, &token);
coda;
}

xapi category_setup()
{
  enter;

  fatal(list_create, &registered, 0);
  fatal(list_create, &registering, 0);
  fatal(list_create, &activated, 0);
  fatal(map_create, &activated_byname);
  fatal(map_create, &activated_byid);

  finally : coda;
}

void category_teardown()
{
  list_ifree(&registered);
  list_ifree(&registering);
  list_ifree(&activated);
  map_ifree(&activated_byname);
  map_ifree(&activated_byid);
}

//
// api
//

API xapi category_say(logger_category * const restrict cat, narrator * restrict N)
{
  enter;

  sayf("%*s : %s", category_name_max_length, cat->name, cat->description);

  finally : coda;
}

API xapi category_report()
{
  enter;

  int token = 0;

  int x;
  for(x = 0; x < list_size(activated); x++)
  {
    // find the bounds of the name-group
    int y;
    for(y = x + 1; y < list_size(activated); y++)
    {
      if(strcmp(list_get(activated, x)->name, list_get(activated, y)->name))
        break;
    }

    fatal(log_start, L_LOGGER, &token);
    narrator * N = log_narrator();
    says(" ");
    fatal(category_say, list_get(activated, x), N);
    fatal(log_finish, &token);

    x = y - 1;
  }

finally:
  fatal(log_finish, &token);
coda;
}

API xapi logger_category_register(logger_category * logs)
{
  enter;

  if(!registered)
    fatal(list_create, &registered, 0);

  if(!registering)
    fatal(list_create, &registering, 0);

  list * tmp = 0;
  fatal(list_create, &tmp, 0);
  while(logs->name)
  {
    fatal(list_push, tmp, logs);
    logs++;
  }

  // merge the registered list with the new list
  fatal(category_list_merge, registered, tmp, registering);

  // swap registering and registered
  void * T = registered;
  registered = registering;
  registering = T;

#if 0
  printf("registered categories\n");
  int x;
  for(x = 0; x < list_size(registered); x++)
  {
    logger_category * this = list_get(registered, x); 
    printf("%*s\n", category_name_max_length, this->name);
  }
#endif

finally:
  list_free(tmp);
coda;
}

API xapi logger_category_activate()
{
  enter;

  category_name_max_length = 0;

  // mask of category ids that have been assigned
  uint64_t used_category_ids_mask = 0;

  list * activating = 0;
  map * activating_byname = 0;
  map * activating_byid = 0;
  list * sublist = 0;

  fatal(list_create, &activating, 0);
  fatal(map_create, &activating_byname);
  fatal(map_create, &activating_byid);

  // merge the activated list with the registered list
  fatal(category_list_merge, activated, registered, activating);

  int x;
  for(x = 0; x < list_size(activating); x++)
  {
    // whether any category in the name-group has no id assigned
    int anyzero = !list_get(activating, x)->id;

    // find the bounds of the name-group
    int y;
    for(y = x + 1; y < list_size(activating); y++)
    {
      if(strcmp(list_get(activating, x)->name, list_get(activating, y)->name))
        break;

      anyzero |= !list_get(activating, y)->id;
    }

    // assign the id as the next unused bit from the mask
    if(~used_category_ids_mask == 0)
      failf(LOGGER_TOOMANY, "limit", "%d", MAX_CATEGORIES);

    uint64_t id = ~used_category_ids_mask & -~used_category_ids_mask;
    used_category_ids_mask |= id;

    // assign the id to all members of the name-group
    int i;
    for(i = x; i < y; i++)
      list_get(activating, i)->id = id;

    logger_category * category = list_get(activating, x);
    char * name = category->name;
    size_t namel = strlen(name);
    category_name_max_length = MAX(category_name_max_length, namel);

    // recalculate name-group attributes if any member was newly added
    if(anyzero)
    {
      // in rank order
      fatal(list_sublist, activating, x, y - x, &sublist);

      int compar(const logger_category * A, const logger_category * B, void * arg)
      {
        return A->rank - B->rank;
      }
      list_sort(sublist, compar, 0);

      uint32_t attr = 0;
      for(i = 0; i < list_size(sublist); i++)
        attr = attr_combine(attr, list_get(sublist, i)->attr);

      for(i = 0; i < list_size(sublist); i++)
      {
        list_get(sublist, i)->namel = namel;
        list_get(sublist, i)->attr = attr;
      }
    }

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

  list_ifree(&registered);
  list_ifree(&registering);

finally:
  list_free(activating);
  map_free(activating_byname);
  map_free(activating_byid);
  list_free(sublist);
coda;
}

API xapi category_byname(const char * const restrict name, size_t namel, logger_category ** const restrict category)
{
  enter;

  namel = namel ?: strlen(name);
  *category = map_get(activated_byname, name, namel);

  finally : coda;
}

API xapi category_byid(uint64_t id, logger_category ** const restrict category)
{
  enter;

  *category = map_get(activated_byid, MM(id));

  finally : coda;
}
