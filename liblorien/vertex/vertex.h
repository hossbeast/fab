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

#ifndef _LORIEN_VERTEX_H
#define _LORIEN_VERTEX_H

/*

MODULE
 vertex

SUMMARY

REMARKS

*/

struct graph;
struct vertex;
typedef struct vertex vertex;

#ifndef VERTEX_VALUE_TYPE 
# define VERTEX_VALUE_TYPE void
#endif

#define restrict __restrict

/// vertex_value_set
//
// SUMMARY
//  set the value for the vertex
//
// PARAMETERS
//  v       - 
//  [value] - 
//
void vertex_value_set(vertex * const restrict v, VERTEX_VALUE_TYPE * value)
  __attribute__((nonnull(1)));

/// vertex_value
//
// SUMMARY
//  get the value for the vertex
//
VERTEX_VALUE_TYPE * vertex_value(vertex * const restrict v)
  __attribute__((nonnull));

#undef restrict
#endif
