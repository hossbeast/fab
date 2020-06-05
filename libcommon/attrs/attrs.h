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

#ifndef _ATTRS_H
#define _ATTRS_H

#include "types.h"

//
// 16
//

typedef struct member16 {
  const char * restrict name;
  uint8_t namel;
  uint16_t range;
  uint16_t value;
} member16;

typedef struct attrs16 {
  uint16_t num;
  member16 * members;
} attrs16;

void attrs16_init(attrs16 * e)
  __attribute__((nonnull));

//
// 32
//

typedef struct member32 {
  const char * restrict name;
  uint8_t namel;
  uint32_t range;
  uint32_t value;
} member32;

typedef struct attrs32 {
  uint16_t num;
  member32 * members;
} attrs32;

void attrs32_init(attrs32 * e)
  __attribute__((nonnull));


//
//
//

/*
 *
 */
uint16_t attrs16_value_byname(const attrs16 * restrict e, const char * restrict name)
  __attribute__((nonnull));

/*
 *
 */
uint16_t attrs16_value_bynamew(const attrs16 * restrict e, const char * restrict name, uint8_t namel)
  __attribute__((nonnull));

/*
 *
 */
const char * attrs16_name_byvalue(const attrs16 * restrict e, uint16_t attrs)
  __attribute__((nonnull));

/*
 *
 */
size_t znload_attrs16(void * restrict dst, size_t sz, const attrs16 * restrict e, uint16_t attrs)
  __attribute__((nonnull));


/*
 *
 */
uint32_t attrs32_value_byname(const attrs32 * restrict e, const char * restrict name)
  __attribute__((nonnull));

/*
 *
 */
uint32_t attrs32_value_bynamew(const attrs32 * restrict e, const char * restrict name, uint8_t namel)
  __attribute__((nonnull));

/*
 *
 */
const char * attrs32_name_byvalue(const attrs32 * restrict e, uint32_t attrs)
  __attribute__((nonnull));

/*
 *
 */
size_t znload_attrs32(void * restrict dst, size_t sz, const attrs32 * restrict e, uint32_t attrs)
  __attribute__((nonnull));

#endif
