/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#ifndef SYSVAR_H
#define SYSVAR_H

#include "xapi.h"

struct exec_builder;
struct exec_render_context;
struct build_slot;

xapi sysvar_builder_variant(struct exec_builder * restrict builder, const struct build_slot * restrict bs)
  __attribute__((nonnull));

xapi sysvar_builder_targets(struct exec_builder * restrict builder, const struct build_slot * restrict bs)
  __attribute__((nonnull));

xapi sysvar_builder_sources(struct exec_builder * restrict builder, const struct build_slot * restrict bs)
  __attribute__((nonnull));

xapi exec_render_sysvar_sources(struct exec_render_context * restrict ctx, const struct build_slot * restrict bs)
  __attribute__((nonnull));

xapi exec_render_sysvar_targets(struct exec_render_context * restrict ctx, const struct build_slot * restrict bs)
  __attribute__((nonnull));

xapi exec_render_sysvar_variant(struct exec_render_context * restrict ctx, const struct build_slot * restrict bs)
  __attribute__((nonnull));

#endif
