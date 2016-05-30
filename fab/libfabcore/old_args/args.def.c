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

#include <stdio.h>

#include "xapi.h"
#include "logger.h"

#include "internal.h"
#include "args.def.h"
#include "selector.def.h"
#include "path.h"
#include "params.h"
#include "logging.internal.h"

#include "memblk.h"
#include "memblk.def.h"
#include "macros.h"

#define restrict __restrict

APIDATA typeof(*g_args) * g_args = 0;

//
// api
//

API void args_freeze(memblk * const restrict mb)
{
  memblk_freeze(mb, &g_args->argvs);
  memblk_freeze(mb, &g_args->buildscript_path);

  int x;

  for(x = 0; x < g_args->bs_runtime_varsl; x++)
    memblk_freeze(mb, &g_args->bs_runtime_vars[x]);
  memblk_freeze(mb, &g_args->bs_runtime_vars);

  for(x = 0; x < g_args->invokedirsl; x++)
    memblk_freeze(mb, &g_args->invokedirs[x]);
  memblk_freeze(mb, &g_args->invokedirs);

  for(x = 0; x < g_args->selectorsl; x++)
    selector_freeze(mb, &g_args->selectors[x]);
  memblk_freeze(mb, &g_args->selectors);

  path_freeze(mb, g_args->init_fabfile_path);
  memblk_freeze(mb, &g_args->init_fabfile_path);
}

API void args_thaw(char * const mb)
{
  int x;

  memblk_thaw(mb, &g_args->argvs);
  memblk_thaw(mb, &g_args->buildscript_path);

  memblk_thaw(mb, &g_args->bs_runtime_vars);
  for(x = 0; x < g_args->bs_runtime_varsl; x++)
    memblk_thaw(mb, &g_args->bs_runtime_vars[x]);

  memblk_thaw(mb, &g_args->invokedirs);
  for(x = 0; x < g_args->invokedirsl; x++)
    memblk_thaw(mb, &g_args->invokedirs[x]);

  memblk_thaw(mb, &g_args->selectors);
  for(x = 0; x < g_args->selectorsl; x++)
    selector_thaw(mb, &g_args->selectors[x]);

  memblk_thaw(mb, &g_args->init_fabfile_path);
  path_thaw(mb, g_args->init_fabfile_path);
}
