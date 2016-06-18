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

#include "permute.h"

static void swap(uint8_t * const restrict set, size_t a, size_t b)
{
  size_t T = set[a];
  set[a] = set[b];
  set[b] = T;
}

static void reverse(uint8_t * const restrict set, size_t i, size_t len)
{
  int x;
  for(x = 0; x < len / 2; x++)
    swap(set, i + x, i + len - 1 - x);
}

//
// public
//

int permute(uint8_t * const restrict set, size_t setl)
{
  int k;
  for(k = setl - 2; k >= 0; k--)
  {
    if(set[k] < set[k + 1])
      break;
  }

  if(k == -1)
    return 0;

  int i;
  for(i = setl - 1; i >  k; i--)
  {
    if(set[k] < set[i])
      break; 
  }

  swap(set, k, i);
  reverse(set, k + 1, setl - (k + 1));

  return 1;
}
