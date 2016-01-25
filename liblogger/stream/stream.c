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

#include "xapi.h"
#include "xlinux.h"
#include "narrate.h"

#include "internal.h"
#include "filter.internal.h"
#include "category.internal.h"

#include "list.h"
#include "macros.h"
#include "strutil.h"

#define restrict __restrict

/// stream
//
// SUMMARY
//  represents an outlet to which log messages are emitted
//
typedef struct stream
{
  // determines which log messages are emitted to the stream
  struct filter *   filter;

  // emitted log messages are written to the narrator
  struct narrator * narrator;

  // attributes for logs emitted to the stream, at higher precedence than both
  // category definitions and attributes at the log site
  uint32_t          attr;
} stream;

// active filters
static stream * active;

//
// static
//

//
// public
//
xapi stream_setup()
{
  enter;

  fatal(list_create, &active, sizeof(stream), 0, 0);

  finally : coda;
}

void stream_teardown()
{
  list_xfree(&active);
}
