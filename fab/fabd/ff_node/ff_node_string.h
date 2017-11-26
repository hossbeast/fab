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

#ifndef FABD_FF_NODE_STRING_H
#define FABD_FF_NODE_STRING_H

#include "ff_node.h"
#include "ff_node_char.h"
#include "ff_node_usrvar.h"
#include "ff_node_word.h"

typedef union ff_node_string_part
{
  union {
    ff_node;
    ff_node base;
  };

  ff_node_char character;
  ff_node_word word;
  ff_node_usrvar usrvar;
} ff_node_string_part;

typedef struct ff_node_string
{
  union {
    ff_node;
    ff_node base;
  };

  ff_node_string_part * chain;
} ff_node_string;

#endif
