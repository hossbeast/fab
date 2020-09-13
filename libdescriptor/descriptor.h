/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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

#ifndef _DESCRIPTOR_H
#define _DESCRIPTOR_H

#include "types.h"

#define DESCRIPTOR_CODE_TABLE                 \
  DEF(DESCRIPTOR_INT      , 1)                \
  DEF(DESCRIPTOR_UINT     , 2)                \
  DEF(DESCRIPTOR_STRING   , 3) /* char* */    \
  DEF(DESCRIPTOR_LIST     , 4)                \

typedef enum descriptor_code {
#undef DEF
#define DEF(x, y) x = y,
DESCRIPTOR_CODE_TABLE
} descriptor_code;

typedef struct descriptor_field {
  const char *name;
  uint8_t name_len;
  uint16_t code;
  uint16_t offset;
  union {
    uint8_t size;             // integer
    uint32_t len_offset;      // string, list
  };
  uint32_t size_offset;       // list
} descriptor_field;

typedef struct descriptor_type {
  uint32_t id;
  const char *name;
  uint8_t name_len;
  uint16_t members_len;
  descriptor_field **members;
} descriptor_type;

/*
 * write a field to a buffer
 */
size_t descriptor_field_marshal(void *restrict dst, size_t sz, descriptor_field *restrict desc, void *restrict object)
  __attribute__((nonnull));

size_t descriptor_marshal_int(void *restrict dst, size_t sz, void * restrict x, uint8_t xsz)
  __attribute__((nonnull));

size_t descriptor_marshal_string(void *restrict dst, size_t sz, void * restrict s, uint16_t slen)
  __attribute__((nonnull));

/*
 * populate an object from marshaled data
 */
size_t descriptor_type_unmarshal(void *restrict object, descriptor_type *restrict desc, void *restrict src, size_t sz)
  __attribute__((nonnull));

#endif
