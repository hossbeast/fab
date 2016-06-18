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

#ifndef _LORIEN_PATHTREE_INTERNAL_H
#define _LORIEN_PATHTREE_INTERNAL_H

#include <sys/types.h>

#include "pathtree.h"

struct list;      // valyria/list.h
struct narrator;  // narrator.h

typedef struct node
{
  char * pfx;
  size_t pfxl;
  size_t keyl; // length of the key represented by this node
  struct list * children;
  PATHTREE_VALUE_TYPE * val;
} node;

struct pathtree
{
  struct list * nodes; // storage
  node * root;
};

/// pathtree_say
//
// SUMMARY
//  write a (multiline) description of a pathtree to the specified narrator
//
// PARAMETERS
//  pt - pathtree
//  N  - narrator
//
xapi pathtree_say(pathtree * pt, struct narrator * const restrict N)
  __attribute__((nonnull));

/// pathtree_search_node
//
// SUMMARY
//  search for a key
//
// PARAMETERS
//  pf  - pathtree
//  key - key
//
// RETURNS
//  node containing a previously inserted key, if any
//
node * pathtree_search_node(pathtree * const restrict pt, char * const restrict key)
  __attribute__((nonnull));

#endif
