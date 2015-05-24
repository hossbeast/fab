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

#ifndef _NARRATE_NARRATOR_H
#define _NARRATE_NARRATOR_H

#include <sys/types.h>

struct pstring;           // pstring.h

typedef struct narrator
{
  struct pstring * ps;    // write to pstring
  char * bb;              // write to fixed-size buffer
  size_t bsz;             // buffer size
  size_t bz;              // buffer offset
  int fd;                 // write to file descriptor
} narrator;

/// narrator
//
// SUMMARY
//  this is the global narration context
//
narrator * _narrator;

#endif
