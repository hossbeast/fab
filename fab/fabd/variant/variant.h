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

#ifndef FABD_VARIANT_H
#define FABD_VARIANT_H

#include "types.h"

typedef struct variant {
  struct variant_tag {
    const char * text;
    uint16_t len;
  } *tags;
  uint16_t tags_len;

  uint16_t norm_len;
  char norm[];        // normalized
} variant;

xapi variant_setup(void);
xapi variant_cleanup(void);

xapi variant_get(const char * restrict name, uint16_t len, variant ** restrict variantp)
  __attribute__((nonnull));

bool variant_key_compatible(
    const variant * restrict var
  , const char * restrict keytext
  , const char ** restrict name
  , uint16_t * restrict name_len
)
  __attribute__((nonnull(2, 3, 4)));

#endif
