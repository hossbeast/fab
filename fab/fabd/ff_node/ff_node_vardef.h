/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#ifndef _FF_NODE_VARDEF_H
#define _FF_NODE_VARDEF_H

#include "ff_node.h"

struct ff_node_varname;
struct ff_node_strings;

typedef struct ff_node_vardef
{
  ff_node;

  struct ff_node_varname * name;
  struct ff_node_strings * def;
} ff_node_vardef;

#endif
