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

#ifndef _VALUE_H
#define _VALUE_H

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

struct pstring;   // libvalyria/pstring
struct list;      // libvalyria/list
struct yyu_location;
struct value_store;
struct narrator;

#define VALUE_TYPE_TABLE(x)       \
  /* aggregates */                \
  VALUE_TYPE(LIST   , 1 , x)      \
  VALUE_TYPE(MAP    , 2 , x)      \
  /* scalars */                   \
  VALUE_TYPE(STRING , 3 , x)      \
  VALUE_TYPE(FLOAT  , 4 , x)      \
  VALUE_TYPE(BOOLEAN, 5 , x)

enum {
#define VALUE_TYPE(a, b, x) VALUE_TYPE_ ## a = (b),
VALUE_TYPE_TABLE(0)
#undef VALUE_TYPE
};

#define VALUE_TYPE(a, b, x) (x) == b ? "VALUE_TYPE_" #a :
#define VALUE_TYPE_STRING(x) VALUE_TYPE_TABLE(x) "UNKNOWN"

typedef struct value_location {
  int f_lin;
  int f_col;
  int l_lin;
  int l_col;

  char * s;
  char * e;
  size_t l;
} value_location;

typedef struct value {
  uint8_t type;           // one of VALUE_TYPE_*
  value_location loc;

  union {
    struct list * l;
    struct {
      struct list * keys;
      struct list * vals;
    };
    struct pstring * s;
    float f;
    int b;
  };
} value;

/// value_load
//
// SUMMARY
//  initialize the library
//
xapi value_load(void);

/// value_unload
//
// SUMMARY
//  release the library
//
xapi value_unload(void);

/// value_say
//
// SUMMARY
//  write a (multiline) description of a value to a narrator
//
// PARAMETERS
//  val - value
//  N   - narrator to write to
//
xapi value_say(const value * const restrict val, struct narrator * const restrict N)
  __attribute__((nonnull(1)));

/// value_say_verbose
//
// SUMMARY
//  write a (multiline) description of a value to a narrator
//
// PARAMETERS
//  val - value
//  N   - narrator to write to
//
xapi value_say_verbose(const value * const restrict val, struct narrator * const restrict N)
  __attribute__((nonnull(1)));

/// value_cmp
//
// SUMMARY
//  compare two value instances
//
int value_cmp(const value * const restrict A, const value * const restrict B);

#endif
