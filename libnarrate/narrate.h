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

#ifndef _NARRATE_H
#define _NARRATE_H

#include <string.h>

// narrator definition, global narration context
#include "narrate/narrator.h"

// narrate_say declarations
#include "narrate/say.h"

//
// declare a function-scope narration
//

//  narrate to a dynamically-resizing buffer
#define narrationp(ps) narrator * _narrator = (typeof(*_narrator)[]){{ .ps = ps }}

//  narrate to a fixed size buffer discarding overflow
#define narrationw(buf, siz) narrator * _narrator = ({   \
  if(siz > 0) { buf[0] = 0; };                                  \
  (typeof(*_narrator)[]){{ .bb = buf, .bsz = siz }};     \
})

// narrate to a file descriptor
#define narrationd(fd) narrator * _narrator = (typeof(*_narrator)[]){{ .fd = fd }}

//
// write to the active narration
//

#define vsayf(fmt, lst)  fatal(narrate_vsayf, _narrator, fmt, lst)
#define sayf(fmt, ...)   fatal(narrate_sayf , _narrator, fmt, ##__VA_ARGS__)
#define says(s)          fatal(narrate_sayw , _narrator, s, strlen(s))
#define sayw(b, l)       fatal(narrate_sayw , _narrator, b, l)
#define sayc(s, c)       fatal(narrate_sayc , _narrator, c)

#endif
