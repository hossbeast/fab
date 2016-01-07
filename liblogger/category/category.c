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

// registered categories, in decreasing order of precedence
static list * registered;
static list * registering;

//static list * resolved;

#define COMPARE_BYNAME 1
#define COMPARE_BYID   2
#define COMPARE_BYRANK 4

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
  logger_category * A = (void*)_A;
  logger_category * B = (void*)_B;
  uint32_t options = (uint32_t)(uintptr_t)_options;

  int r = 0;
  if(r == 0 && (options & COMPARE_BYNAME))
    r = strcmp(A->name, B->name);

  if(r == 0 && (options & COMPARE_BYID))
  {
    if(A->id > B->id)
      r = 1;
    else if(A->id < B->id)
      r = -1;
  }

  if(r == 0 && (options & COMPARE_BYRANK))
    r = A->rank - B->rank;

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
// public
//
xapi logger_category_setup()
{
  enter;

  fatal(list_create, &registered, sizeof(logger_category *), 0, LIST_DEREF);
  fatal(list_create, &registering, sizeof(logger_category *), 0, LIST_DEREF);

  finally : coda;
}

void logger_category_teardown()
{
}

//
// api
//

/*
static xapi current_flush(map * const restrict current, map * const restrict previous, char * const restrict last)
{
  enter;

  int x;
  for(x = 0; x < map_slots(current); x++)
  {
    char * name = map_keyat(current, x);
    if(name)
    {
      if(map_get(previous, MMS(name)))
      {

printf("last\n");
printf(" %s\n", last);
int i;
printf("previous\n");
for(i = 0; i < map_slots(previous); i++)
{
  char * e;
  if((e = map_keyat(previous, i)))
    printf(" %s\n", e);
}

printf("current\n");
for(i = 0; i < map_slots(current); i++)
{
  char * e;
  if((e = map_keyat(current, i)))
    printf(" %s\n", e);
}

        failf(LOGGER_ILLORDER, "category %s registered with opposite ordering relative to %s", name, last);
      }
      else
      {
        fatal(map_set, previous, MMS(name), 0, 0, 0);
      }
    }
  }

  map_clear(current);

  finally : coda;
}
*/

API xapi logger_category_register(logger_category * logs, char * const restrict identity)
{
  enter;

printf("registering\n");

  map * common = 0;
  int * ax = 0;
  int * bx = 0;

  list_clear(registering);

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
      location * loc = 0;
      if(!(loc = map_get(common, MMS(this->name))))
      {
        fatal(map_set, common, MMS(this->name), 0, sizeof(location), &loc);
        loc->Bx = -1;
      }
      loc->Ax = x;

      x++;

      fatal(list_append, registering, &this);
    }

    if(logs[y].name)
    {
      logger_category * this = &logs[y];
      location * loc = 0;
      if(!(loc = map_get(common, MMS(this->name))))
      {
        fatal(map_set, common, MMS(this->name), 0, sizeof(location), &loc);
        loc->Ax = -1;
      }
      loc->Bx = y;

      y++;

      fatal(list_append, registering, &this);
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

  int compar(const void * A, const void * B) { return *(int*)A - *(int*)B; };
  qsort(ax, c, sizeof(*ax), compar);
  qsort(bx, c, sizeof(*bx), compar);

  for(x = 0; x < c; x++)
  {
    if(strcmp(list_get(registered, ax[x])->name, logs[bx[x]].name))
    {
      failf(LOGGER_ILLORDER, "category %s registered with opposite ordering relative to %s", list_get(registered, ax[x])->name, logs[bx[x]].name);
    }
  }

  void * T = registered;
  registered = registering;
  registering = T;

  printf("registered categories\n");
  for(x = 0; x < list_size(registered); x++)
  {
    logger_category * this = list_get(registered, x); 

    char space[64];
    narrationw(space, sizeof(space));

    sayf("%*s : 0x%016"PRIx64 " 0x%08"PRIx32 " ", category_name_max_length + 7, this->name, this->id, this->attr);
    fatal(category_attr_say, this->attr, _narrator);
    printf("%s\n", space);
  }

finally:
  if(XAPI_THROWING(LOGGER_ILLORDER))
  {
    XAPI_INFOF("identity", "%s", identity);
  }

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
      if(category_compare(first, list_get(registered, y), (void*)COMPARE_BYNAME))
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

#if 0
  printf("category definitions\n");
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

    sayf("%*s : 0x%016"PRIx64 " 0x%08"PRIx32 " ", category_name_max_length, resolved.v[x]->name, resolved.v[x]->id, resolved.v[x]->attr);
    fatal(category_attr_say, resolved.v[x]->attr, _narrator);
    printf("%s\n", space);

    x = y - 1;
  }
#endif

  finally : coda;
}

#if 0
// registered but not yet resolved
static union {
  list;

  struct {
    logger_category **  v;
    size_t              l;
  };
} unresolved = {{ list_es : sizeof(logger_category *) }};

// registration identities
static union {
  list;

  struct {
    pstring ** v;
    size_t     l;
  };
} identities = {{ list_es : sizeof(char*) }};

// resolved categories, sorted by name, rank
static union {
  list;

  struct {
    logger_category **  v;
    size_t              l;
  };
} resolved = {{ list_es : sizeof(logger_category *) }};


API xapi logger_category_register(logger_category * logs, char * const restrict identity)
{
  enter;

  pstring * ps = 0;
  fatal(psloads, &ps, identity);
  fatal(list_append, &unresolved, &logs);
  fatal(list_append, &identities, &ps);
  ps = 0;

finally:
  psfree(ps);
coda;
}
#endif
