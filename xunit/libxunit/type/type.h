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

#ifndef _XUNIT_TYPE_H
#define _XUNIT_TYPE_H

/*

MODULE
 xunit/type

SUMMARY

*/

#include <sys/types.h>
#include <stdint.h>

typedef union xunit_arg
{
  struct {      // xunit_buffer
    void * p;
    size_t l;
  };

  int d;        // xunit_int
  char * s;     // xunit_string
  xapi e;       // xunit_xapi
  int64_t i64;  // xunit_int64
  float f;      // xunit_float
  char b;       // xunit_bool

  void * udata;
} xunit_arg;

typedef struct xunit_type
{
  void (*xu_unpack)(va_list, xunit_arg *);
  int (*xu_compare)(xunit_arg *, xunit_arg *);
  void (*xu_info_add)(const char *, xunit_arg *);
} xunit_type;

#endif
