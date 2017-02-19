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

#ifndef _MACROS_H
#define _MACROS_H

#include <string.h>

#define QUOTE(x) #x
#define XQUOTE(x) QUOTE(x)

#define MIN(a,b)            \
 ({ typeof (a) _a = (a);    \
     typeof (b) _b = (b);   \
   _a > _b ? _b : _a; })

#define MAX(a,b)            \
 ({ typeof (a) _a = (a);    \
     typeof (b) _b = (b);   \
   _a > _b ? _a : _b; })

/// sentinel
//
// SUMMARY
//  counts the number of elements in an array terminated by a zeroed-entry
//
// RETURNS
//  size_t
//
#define sentinel(x) ({                                \
  typeof((x)[0]) * p = (x);                           \
  while(memcmp(p, (char[sizeof(*p)]){}, sizeof(*p)))  \
    p++;                                              \
  p - (x);                                            \
})

/// NARGS
//
// SUMMARY
//  evaluates to the number of arguments passed to it
//
#define NARGS(...) ((sizeof((int[]){0, ##__VA_ARGS__}) / sizeof(int)) - 1)

/// MM - helper macro
//
// many map api's require an item and its length (get, set, delete)
//
// when the items size is known at compile time, MM makes it simpler to pass
//
#define MM(x) (void*)&(x), sizeof(x)
#define MMS(x) (x), strlen(x)

/// containerof
//
// SUMMARY
//  get a pointer to a containing structure from an inner structure
//
#define containerof(ptr, type, member) ({               \
  const typeof(((type*)0)->member) * __mptr = (ptr);    \
  (type*)((char*)__mptr - offsetof(type, member));      \
})

/// memncmp
//
// SUMMARY
//  compare two regions of memory
//
// PARAMETERS
//  a    - pointer to the first region
//  alen - size of the first region
//  b    - pointer to the second region
//  blen - size of the second region
//
// RETURNS
//  a la strcmp
//
static inline int memncmp(const void * a, size_t alen, const void * b, size_t blen)
{
  int r = memcmp(a, b, MIN(alen, blen));
  if(r != 0)
    return r;
  if(alen > blen)
    return 1;
  if(blen > alen)
    return -1;
  return 0;
}

#endif
