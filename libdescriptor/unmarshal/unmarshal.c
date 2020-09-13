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

#include <string.h>

#include "descriptor.h"
#include "macros.h"

//
// static
//

static size_t field_unmarshal_int(void * restrict src, size_t sz, void * restrict dst, uint8_t size)
{
  uint32_t pad;
  size_t z;

  z = 0;
  pad = alignp(src, size);
  RUNTIME_ASSERT((sz - z) >= pad);
  z += pad;

  memcpy(dst, src + z, size);
  RUNTIME_ASSERT((sz - z) >= size);
  z += size;

  return z;
}

static size_t field_unmarshal_string(void * restrict src, size_t sz, char ** textp, uint16_t *lenp)
{
  uint32_t pad;
  size_t z;

  z = 0;
  pad = alignp(src, sizeof(*lenp));
  RUNTIME_ASSERT((sz - z) >= pad);
  z += pad;

  // length
  memcpy(lenp, src + z, sizeof(*lenp));
  RUNTIME_ASSERT((sz - z) >= sizeof(*lenp));
  z += sizeof(*lenp);

  // data
  *textp = src + z;
  RUNTIME_ASSERT((sz - z) >= *lenp);
  z += *lenp;

  return z;
}

static size_t field_unmarshal_list(void * restrict src, size_t sz, void ** listp, uint32_t * sizep, uint16_t *lenp)
{
  uint32_t pad;
  uint32_t size;
  uint16_t len;
  size_t z;

  z = 0;
  pad = alignp(src, sizeof(size));
  RUNTIME_ASSERT((sz - z) >= pad);
  z += pad;

  // size
  memcpy(&size, src + z, sizeof(size));
  RUNTIME_ASSERT(size >= (sizeof(size) + sizeof(len)));
  z += sizeof(size);

  // length
  memcpy(&len, src + z, sizeof(len));
  RUNTIME_ASSERT((sz - z) >= sizeof(len));
  z += sizeof(len);

  *listp = src + z;
  *sizep = size;
  *lenp = len;

  return pad + size;
}

//
// API
//

size_t API descriptor_type_unmarshal(void *restrict object, descriptor_type *restrict desc, void *restrict src, size_t sz)
{
  uint32_t id;
  descriptor_field *field;
  size_t z;
  int x;

  z = 0;
  RUNTIME_ASSERT((sz - z) >= sizeof(id));
  z += alignp(src, sizeof(id));

  z += field_unmarshal_int(src + z, sz - z, &id, sizeof(id));
  RUNTIME_ASSERT(id == desc->id);

  for(x = 0; x < desc->members_len; x++)
  {
    field = desc->members[x];
    if(field->code == 1 || field->code == 2)
    {
      z += field_unmarshal_int(src + z, sz - z, object + field->offset, field->size);
    }
    else if(field->code == 3)
    {
      z += field_unmarshal_string(src + z, sz - z, object + field->offset, object + field->len_offset);
    }
    else if(field->code == 4)
    {
      z += field_unmarshal_list(src + z, sz - z, object + field->offset, object + field->size_offset, object + field->len_offset);
    }
    else
    {
      RUNTIME_ABORT();
    }
  }

  return z;
}
