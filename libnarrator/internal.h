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

#ifndef _NARRATOR_INTERNAL_H
#define _NARRATOR_INTERNAL_H

#include "narrator.h"

// other declarations
#define API __attribute__((visibility("protected")))
#define APIDATA

// structure definitions
#include "fd.internal.h"
#include "fixed.internal.h"
#include "growing.internal.h"
#include "multi.internal.h"
#include "nullity.internal.h"
#include "record.internal.h"
#include "rolling.internal.h"

#include "macros.h"

#define NARRATOR_TYPE_TABLE       \
  NARRATOR_TYPE_DEF(FIXED)        \
  NARRATOR_TYPE_DEF(GROWING)      \
  NARRATOR_TYPE_DEF(FD)           \
  NARRATOR_TYPE_DEF(MULTI)        \
  NARRATOR_TYPE_DEF(NULLITY)      \
  NARRATOR_TYPE_DEF(RECORD)       \
  NARRATOR_TYPE_DEF(ROLLING)      \

typedef enum narrator_type {
  NARRATOR_TYPE_RANGE_BEFORE = 0,

#define NARRATOR_TYPE_DEF(x) NARRATOR_ ## x,
NARRATOR_TYPE_TABLE
#undef NARRATOR_TYPE_DEF

  NARRATOR_TYPE_RANGE_AFTER
} narrator_type;

/// narrator
//
// SUMMARY
//  primary data type
//
typedef struct narrator
{
  narrator_type type;       // one of NARRATOR_*

  union {
    narrator_fixed fixed;
    narrator_growing growing;
    narrator_fd fd;
    narrator_multi multi;
    narrator_nullity nullity;
    narrator_record record;
    narrator_rolling rolling;
  };
} narrator;

STATIC_ASSERT(NARRATOR_STATIC_SIZE == sizeof(narrator));

#endif
