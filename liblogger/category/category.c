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
#include "valyria/array.h"
#include "valyria/map.h"

#include "internal.h"
#include "category.internal.h"
#include "LOGGER.errtab.h"
#include "log.internal.h"
#include "attr.internal.h"
#include "logging.internal.h"

#include "macros.h"
#include "zbuffer.h"

#define restrict __restrict

int category_name_max_length;
uint64_t category_optional_mask;

// maximum number of unique categories by name
#define MAX_CATEGORIES  (sizeof(((logger_category*)0)->id) * 8)

// list of registered category lists
static list * registered;

// activated categories, in decreasing order of precedence
static list * activated;

static map * activated_byname;
static map * activated_byid;

//
// static
//

/// select_category
//
// SUMMARY
//  copy instances of a category from source lists to the dst list
//
static xapi select_category(list * restrict lists, dictionary * restrict index, dictionary * restrict selections, list * restrict dst, const char * restrict name)
{
  enter;

  int * state;
  int * indexes;
  int x;
  int y;

  if((state = dictionary_get(selections, MMS(name))) == 0)
    fatal(dictionary_set, selections, MMS(name), &state);

  // this category was already selected
  if(*state == 2)
    goto XAPI_FINALIZE;

  // this category is being selected in another frame
  else if(*state == 1)
  {
    char space[128];
    char * s = space;
    size_t sz = sizeof(space);
    size_t z = 0;

    for(x = 0; x < dictionary_table_size(selections); x++)
    {
      const char * key;
      if((key = dictionary_table_key(selections, x)))
      {
        int * val = dictionary_table_value(selections, x);
        if(*val == 1)
        {
          if(z)
            z += znloads(s + z, sz - z, ", ");
          z += znloads(s + z, sz - z, key);
        }
      }
    }

    failw(LOGGER_ILLORDER, "categories", s, z);
  }

  *state = 1;
  indexes = dictionary_get(index, MMS(name));

  for(x = 0; x < lists->l; x++)
  {
    list * A = list_get(lists, x);

    for(y = 0; y < indexes[x]; y++)
    {
      logger_category * cat = list_get(A, y);
      fatal(select_category, lists, index, selections, dst, cat->name);
    }
  }

  for(x = 0; x < lists->l; x++)
  {
    if(indexes[x] < 0)
      continue;

    list * A = list_get(lists, x);
    fatal(list_replicate, dst, dst->l, A, indexes[x], 1);
  }

  *state = 2;

  finally : coda;
}

//
// public
//

xapi category_setup()
{
  enter;

  fatal(list_createx, &registered, 0, list_xfree, 0);
  fatal(list_create, &activated);
  fatal(map_create, &activated_byname);
  fatal(map_create, &activated_byid);

  finally : coda;
}

xapi category_cleanup()
{
  enter;

  fatal(list_ixfree, &registered);
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

xapi category_lists_merge(list * restrict lists, list * restrict dst)
{
  enter;

  dictionary * selections = 0;    // per category selection state
  dictionary * index = 0;         // per category per list element index

  int x;
  int y;

  fatal(dictionary_create, &index, sizeof(int) * lists->l);
  fatal(dictionary_create, &selections, sizeof(int));

  // index = { category-name : [ <element-index> @ <list-index> ] }
  //  mapping from element name to per-list index of its occurrence
  for(x = 0; x < lists->l; x++)
  {
    list * listp = list_get(lists, x);
    for(y = 0; y < listp->l; y++)
    {
      logger_category * cat = list_get(listp, y);
      int * indexes = 0;
      if((indexes = dictionary_get(index, MMS(cat->name))) == 0)
      {
        fatal(dictionary_set, index, MMS(cat->name), &indexes);

        int i;
        for(i = 0; i < lists->l; i++)
          indexes[i] = -1;
      }

      if(indexes[x] != -1)
        fails(LOGGER_ILLREPEAT, "category", cat->name);

      indexes[x] = y;
    }
  }

  for(x = 0; x < lists->l; x++)
  {
    list * A = list_get(lists, x);
    for(y = 0; y < A->l; y++)
    {
      logger_category * cat = list_get(A, y);
      fatal(select_category, lists, index, selections, dst, cat->name);
    }
  }

finally:
  fatal(dictionary_xfree, selections);
  fatal(dictionary_xfree, index);
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

  // merge the registered lists, preserving intra-list dependencies
  fatal(category_lists_merge, registered, activating);

  int x;
  for(x = 0; x < list_size(activating); x++)
  {
    // whether any category in the name-group has no id assigned
    int anyzero = !((logger_category *)list_get(activating, x))->id;

    // find the bounds of the name-group
    logger_category * A = list_get(activating, x);

    int y;
    for(y = x + 1; y < list_size(activating); y++)
    {
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
    {
      logger_category * cat = list_get(activating, i);
      cat->id = id;
    }

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

  list * tmp = 0;

  fatal(list_create, &tmp);
  while(logs->name)
  {
    fatal(list_push, tmp, logs);
    logs++;
  }

  fatal(list_push, registered, tmp);
  tmp = 0;

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
