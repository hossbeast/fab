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

#include <stdarg.h>

#include "xapi.h"
#include "xunit/type.h"

#include "ff_node.h"

//
// types
//

static void ffn_type_unpack(va_list va, xunit_arg * a)
{
  a->d = va_arg(va, int);
}

static int ffn_type_compare(xunit_arg * A, xunit_arg * B)
{
  return A->d - B->d;
}

static void ffn_type_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushs(name, ffn_type_name(a->d));
}

//
// API
//

xunit_type * xunit_ffn_type = (xunit_type[]) {{
    xu_unpack : ffn_type_unpack
  , xu_compare : ffn_type_compare
  , xu_info_push : ffn_type_info_push
}};
