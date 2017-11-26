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

#ifndef FF_NODE_ARTIFACT_PATTERN_H
#define FF_NODE_ARTIFACT_PATTERN_H

#include "ff_node.h"

#include "ff_node_alternation.h"
#include "ff_node_char.h"
#include "ff_node_class.h"
#include "ff_node_dirsep.h"
#include "ff_node_usrvar.h"
#include "ff_node_word.h"

typedef union ff_node_artifact_pattern_part
{
  union {
    ff_node;
    ff_node base;
  };

  ff_node_alternation alternation;
  ff_node_char character;
  ff_node_class class;
  ff_node_dirsep dirsep;
  ff_node_usrvar usrvar;
  ff_node_word word;
} ff_node_artifact_pattern_part;

typedef struct ff_node_artifact_pattern
{
  union {
    ff_node;
    ff_node base;
  };

  ff_node_artifact_pattern_part * chain;
} ff_node_artifact_pattern;

#endif
