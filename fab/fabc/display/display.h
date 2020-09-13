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

#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "xapi.h"
#include "types.h"

struct fabipc_message;
struct fab_client;

enum {
  REBIND = 1,
  EXIT,
};

typedef struct display {
  xapi (*setup)(void);
  xapi (*redrive)(struct fab_client * restrict client);
  void (*rebind)(const struct fabipc_message * restrict msg);
  void (*result)(const struct fabipc_message * restrict msg);
  xapi (*draw)(void);
  int (*keypress)(int key);
} display;

extern struct display * g_display;

xapi display_switch(display * restrict dis)
  __attribute__((nonnull));

#endif
