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

struct pstring;       // libvalyria/pstring
struct list;          // libvalyria/list
struct yyu_location;
struct value_store;
struct narrator;

#define VALUE_TYPE_TABLE(x)           \
  /* aggregates */                    \
  VALUE_TYPE(LIST   , 0x010 , x)      \
  VALUE_TYPE(MAP    , 0x020 , x)      \
  /* scalars */                       \
  VALUE_TYPE(STRING , 0x100 , x)      \
  VALUE_TYPE(FLOAT  , 0x201 , x)      \
  VALUE_TYPE(BOOLEAN, 0x300 , x)      \
  VALUE_TYPE(INTEGER, 0x401 , x)

enum {
#define VALUE_TYPE(a, b, x) VALUE_TYPE_ ## a = UINT16_C(b),
VALUE_TYPE_TABLE(0)
#undef VALUE_TYPE
};

#define VALUE_TYPE(a, b, x) (x) == b ? "VALUE_TYPE_" #a :
#define VALUE_TYPE_STRING(x) VALUE_TYPE_TABLE(x) "UNKNOWN"

#define VALUE_TYPE_SCALAR     0xF00
#define VALUE_TYPE_AGGREGATE  0x0F0

/*
 * options and modifiers
 */
#define VALUE_ATTR_TABLE(x, y)                                                  \
  VALUE_ATTR_DEF(MERGE_ADD     , 0x0000 , x , y)  /* (default) add elements */  \
  VALUE_ATTR_DEF(MERGE_SET     , 0x0001 , x , y)  /* set elements */            \

enum {
#define VALUE_ATTR_DEF(a, b, x, y) VALUE_ ## a = UINT16_C(b),
VALUE_ATTR_TABLE(0, 0)
#undef VALUE_ATTR_DEF
};

typedef struct value_location {
  const char * fname;

  int f_lin;
  int f_col;
  int l_lin;
  int l_col;

  char * s;
  char * e;
  size_t l;
} value_location;

typedef struct value {
  uint16_t type;           // one of VALUE_TYPE_*
  value_location loc;
  uint16_t attr;           // bitwise combo of VALUE_*

  union {
    struct list * els;      // list
    struct {                // map
      struct list * keys;
      struct list * vals;
    };
    struct pstring * s;     // string
    double f;               // float
    uint8_t b;              // bool
    int64_t i;              // integer
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

/// value_hash
//
// SUMMARY
//  compute a hash over the value
//
uint64_t value_hash(const value * const restrict val)
  __attribute__((nonnull));

#endif
