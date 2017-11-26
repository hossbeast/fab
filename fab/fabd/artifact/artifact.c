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

#include "types.h"
#include "xapi.h"

#include "valyria/list.h"

#include "artifact.internal.h"
#include "ff_node_artifact_pattern.internal.h"
#include "node.h"
#include "pattern/generate.internal.h"

//
// public
//

xapi artifact_pattern_generate(
    const ff_node_artifact_pattern * restrict pat
  , node * restrict context_node
  , list * restrict results
)
{
  enter;

  fatal(pattern_generate, (struct ff_node_pattern*)pat, context_node, 0, 0, 0, results, true);

  finally : coda;
}
