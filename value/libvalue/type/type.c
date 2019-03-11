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

#include "types.h"

#include "internal.h"
#include "type.internal.h"

#include "attrs.h"

APIDATA attrs32 * value_type_attrs = (attrs32[]){{
#define VALUE_TYPE_DEF(x, r, y) + 1
    num : 0 VALUE_TYPE_TABLE
  , members : (member32[]) {
#undef VALUE_TYPE_DEF
#define VALUE_TYPE_DEF(x, r, y) { name : #x, value : UINT32_C(y), range : UINT32_C(r) },
VALUE_TYPE_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(value_type_attrs);
}
