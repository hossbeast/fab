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

#ifndef _NARRATE_INTERNAL_H
#define _NARRATE_INTERNAL_H

#include <stdint.h>

// public header file
#include "narrator.h"

// structure definitions
#include "fixed.internal.h"
#include "growing.internal.h"
#include "file.internal.h"

// other declarations
#define API __attribute__((visibility("protected")))
#define APIDATA

#define NARRATOR_TYPE_TABLE(x)     \
  NARRATOR_TYPE_DEF(FIXED  , 1, x) \
  NARRATOR_TYPE_DEF(GROWING, 2, x) \
  NARRATOR_TYPE_DEF(FILE   , 3, x)

enum {
#define NARRATOR_TYPE_DEF(a, b, x) NARRATOR_ ## a = b,
NARRATOR_TYPE_TABLE(0)
#undef NARRATOR_TYPE_DEF
};

/// narrator
//
// SUMMARY
//  primary data type
//
typedef struct narrator
{
  union {
    narrator_fixed fixed;
    narrator_growing growing;
    narrator_file file;
  };

  uint8_t type;       // one of NARRATOR_*
} narrator;

#endif
