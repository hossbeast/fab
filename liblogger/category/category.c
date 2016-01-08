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

#include <inttypes.h>

#include "xapi.h"
#include "xlinux.h"
#include "narrate.h"
#include "pstring.h"

#include "internal.h"
#include "category.internal.h"
#include "log.internal.h"

#define LIST_ELEMENT_TYPE logger_category*
#include "list.h"
#include "map.h"
#include "macros.h"

#define restrict __restrict

int category_name_max_length;

// maximum number of unique categories by name
#define MAX_CATEGORIES  (sizeof(((logger_category*)0)->id) * 8)

//
// static
//

// mask of all category ids which have already been used
static uint64_t used_category_ids_mask;

// categories that have been registered but not yet resolved
static list * registered;

// active categories, in decreasing order of precedence
static list * categories;

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

//
// public
//

xapi category_setup()
{
  enter;

  fatal(list_create, &registered, sizeof(logger_category *), 0, LIST_DEREF);
  fatal(list_create, &categories, sizeof(logger_category *), 0, LIST_DEREF);

  finally : coda;
}

void category_teardown()
{
  list_free(registered);
  list_free(categories);
}

/// category_attr_say
//
// SUMMARY
//  write a description of attr to the narrator
//
xapi category_attr_say(uint32_t attr, narrator * _narrator)
{
  enter;

  if(attr & COLOR_OPT)
    says(LOGGER_COLOR_VALUE(attr));

  if(attr & PREFIX_OPT)
    says(LOGGER_PREFIX_VALUE(attr));

  if(attr & TRACE_OPT)
    says(LOGGER_TRACE_VALUE(attr));

  if(attr & DISCOVERY_OPT)
    says(LOGGER_DISCOVERY_VALUE(attr));

  finally : coda;
}

//
// api
//

API xapi logger_category_register(logger_category * logs, char * const restrict identity)
{
  enter;

  list * registering = 0;
  map * common = 0;
  int * ax = 0;
  int * bx = 0;

  fatal(list_create, &registering, sizeof(logger_category *), 0, LIST_DEREF);

  // build a map of elements in common between the two lists
  fatal(map_create, &common, 0);

  typedef struct {
    int Ax;
    int Bx;
  } location;

  int x = 0;
  int y = 0;
  while(x < list_size(registered) || logs[y].name)
  {
    if(x < list_size(registered))
    {
      logger_category * this = list_get(registered, x);
      location * loc;
      if(!(loc = map_get(common, MMS(this->name))))
      {
        fatal(map_set, common, MMS(this->name), 0, sizeof(location), &loc);
        loc->Bx = -1;
      }
      loc->Ax = x++;
    }

    if(logs[y].name)
    {
      logger_category * this = &logs[y];
      location * loc;
      if(!(loc = map_get(common, MMS(this->name))))
      {
        fatal(map_set, common, MMS(this->name), 0, sizeof(location), &loc);
        loc->Ax = -1;
      }
      loc->Bx = y++;
    }
  }

  fatal(xmalloc, &ax, sizeof(*ax) * map_size(common));
  fatal(xmalloc, &bx, sizeof(*ax) * map_size(common));

  int c = 0;
  for(x = 0; x < map_slots(common); x++)
  {
    location * loc = 0;
    if((loc = map_valueat(common, x)))
    {
      if(loc->Ax >= 0 && loc->Bx >= 0)
      {
        ax[c] = loc->Ax;
        bx[c] = loc->Bx;
        c++;
      }
    }
  }

  // sort the per-list indices
  int compar(const void * A, const void * B) { return *(int*)A - *(int*)B; };
  qsort(ax, c, sizeof(*ax), compar);
  qsort(bx, c, sizeof(*bx), compar);

  // a conflict exists if any two common elements appear in opposite order
  for(x = 0; x < c; x++)
  {
    if(strcmp(list_get(registered, ax[x])->name, logs[bx[x]].name))
    {
      failf(LOGGER_ILLORDER, "category %s registered with opposite ordering relative to %s", list_get(registered, ax[x])->name, logs[bx[x]].name);
    }
  }

  // write the registered list with the guarantee that common elements appear
  // in the same order
  x = 0;
  y = 0;
  int cx = 0;
  while(x < list_size(registered) || logs[y].name)
  {
    if(cx < c && (x == ax[cx] && y == bx[cx]))
    {
      logger_category * this = list_get(registered, x++);
      fatal(list_append, registering, &this);

      this = &logs[y++];
      fatal(list_append, registering, &this);
      cx++;
    }

    if((cx == c || x < ax[cx]) && x < list_size(registered))
    {
      logger_category * this = list_get(registered, x++);
      fatal(list_append, registering, &this);
    }
    if((cx == c || y < bx[cx]) && logs[y].name)
    {
      logger_category * this = &logs[y++];
      fatal(list_append, registering, &this);
    }
  }

  void * T = registered;
  registered = registering;
  registering = T;

#if 0
  printf("registered categories\n");
  for(x = 0; x < list_size(registered); x++)
  {
    logger_category * this = list_get(registered, x); 
    printf("%*s\n", category_name_max_length, this->name);
  }
#endif

finally:
  if(XAPI_THROWING(LOGGER_ILLORDER))
  {
    XAPI_INFOF("identity", "%s", identity);
  }

  list_free(registering);
  map_free(common);
  free(ax);
  free(bx);
coda;
}

xapi logger_category_resolve()
{
  enter;

  int x;
  for(x = 0; x < list_size(registered); x++)
  {
    logger_category * first = list_get(registered, x); 

    // whether any category in the name-group has no id assigned
    uint64_t id = first->id;
    int anyzero = !first->id;

    // find the bounds of the name-group
    int y;
    for(y = x + 1; y < list_size(registered); y++)
    {
      if(strcmp(list_get(registered, x)->name, list_get(registered, y)->name))
        break;

      id |= list_get(registered, y)->id;
      anyzero |= !list_get(registered, y)->id;
    }

    // recalculate attributes if any member was newly added to the name-group
    if(anyzero)
    {
      size_t namel = strlen(first->name);

      // assign the next unused bit
      if(!id)
      {
        fatal(next_category_id, &id);
        category_name_max_length = MAX(category_name_max_length, namel);
      }

      uint32_t attr = 0;

      int i;
      for(i = x; i < y; i++)
      {
        logger_category * this = list_get(registered, i);

        if(this->attr & COLOR_OPT)
        {
          attr &= ~COLOR_OPT;
          attr |= (this->attr & COLOR_OPT);
        }

        if(this->attr & PREFIX_OPT)
        {
          attr &= ~PREFIX_OPT;
          attr |= (this->attr & PREFIX_OPT);
        }

        if(this->attr & TRACE_OPT)
        {
          attr &= ~TRACE_OPT;
          attr |= (this->attr & TRACE_OPT);
        }

        if(this->attr & DISCOVERY_OPT)
        {
          attr &= ~DISCOVERY_OPT;
          attr |= (this->attr & DISCOVERY_OPT);
        }

        this->namel = namel;
      }

      for(i = x; i < y; i++)
      {
        logger_category * this = list_get(registered, i);

        this->id = id;
        this->attr = attr;
      }
    }

    x = y - 1;
  }

#if 1
  printf("logging categories\n");
  for(x = 0; x < list_size(registered); x++)
  {
    // find the bounds of the name-group
    int y;
    for(y = x + 1; y < list_size(registered); y++)
    {
      if(strcmp(list_get(registered, x)->name, list_get(registered, y)->name))
        break;
    }

    char space[64];
    narrationw(space, sizeof(space));

    sayf("%*s : 0x%016"PRIx64 " 0x%08"PRIx32 " ", category_name_max_length, list_get(registered, x)->name, list_get(registered, x)->id, list_get(registered, x)->attr);
    fatal(category_attr_say, list_get(registered, x)->attr, _narrator);
    printf("%s\n", space);

    x = y - 1;
  }
#endif

  finally : coda;
}
