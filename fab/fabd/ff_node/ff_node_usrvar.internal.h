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

#ifndef _FF_NODE_USRVAR_INTERNAL_H
#define _FF_NODE_USRVAR_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "ff_node_usrvar.h"

struct narrator;
struct ffn_render_context;

xapi ffn_usrvar_say_tree(const ff_node_usrvar * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

xapi ffn_usrvar_say_normal(const ff_node_usrvar * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

xapi ffn_usrvar_render(const ff_node_usrvar * restrict n, struct ffn_render_context * restrict ctx, struct narrator * restrict N)
  __attribute__((nonnull));

xapi ffn_usrvar_mknode(ff_node_usrvar ** restrict n, va_list va)
  __attribute__((nonnull));

void ffn_usrvar_destroy(ff_node_usrvar * restrict n)
  __attribute__((nonnull));

#endif
