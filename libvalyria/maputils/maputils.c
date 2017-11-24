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

#include "xapi.h"

#include "internal.h"
#include "maputils.internal.h"
#include "faults.internal.h"

/// maputils_hashkey
//
// compute the hash of a key
//
// REMARKS
//  the jenkins hash function : http://en.wikipedia.org/wiki/Jenkins_hash_function
//
size_t maputils_hashkey(const char * restrict k, size_t kl, size_t lm)
{
  size_t x;
  size_t h = 0;

  if(fault(MAPDEF_HASH_BOUNDARY_ALL))
  {
    return lm;
  }
  else if(fault(MAPDEF_HASH_BOUNDARY_KEY))
  {
    for(x = 0; x < fault_state.mapdef_hash_boundary.len; x++)
    {
      typeof(*fault_state.mapdef_hash_boundary.keys) faultkey = fault_state.mapdef_hash_boundary.keys[x];

      if(kl == faultkey.len && memcmp(k, faultkey.key, kl) == 0)
        return lm;
    }
  }

  for(x = 0; x < kl; x++)
  {
    h += k[x];
    h += (h << 10);
    h ^= (h >> 6);
  }
  h += (h << 3);
  h ^= (h >> 11);
  h += (h << 15);

  return h & lm;
}
