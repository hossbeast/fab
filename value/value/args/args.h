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

#ifndef _ARGS_H
#define _ARGS_H

#include "xapi.h"
#include "types.h"

enum value_type;

typedef enum {
  INPUT_FILE = 1,
  INPUT_TEXT
} input_type;

extern struct g_args_t {
  struct {
    input_type type;
    enum value_type container;
    char * s;
  } * inputs;
  uint16_t inputsl;
} g_args;

xapi args_parse(void);
xapi args_summarize(void);
void args_teardown(void);

#endif
