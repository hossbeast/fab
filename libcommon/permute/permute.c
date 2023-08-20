/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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

bool API permute(uint8_t * const restrict set, size_t setl)
{
  int k;
  for(k = setl - 2; k >= 0; k--)
  {
    if(set[k] < set[k + 1])
      break;
  }

  if(k == -1)
    return false;

  int i;
  for(i = setl - 1; i >  k; i--)
  {
    if(set[k] < set[i])
      break;
  }

  swap(set, k, i);
  reverse(set, k + 1, setl - (k + 1));

  return true;
}
