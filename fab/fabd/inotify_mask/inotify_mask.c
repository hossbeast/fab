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

#include <sys/inotify.h>

#include "narrator.h"

#include "inotify_mask.h"

xapi inotify_mask_say(uint32_t m, struct narrator * N)
{
  enter;

  int said = 0;
  while(m)
  {
    char* s = 0;
    uint32_t b = 0;
                                                                                          // node operations
         if(m & IN_ACCESS)					{ s = "IN_ACCESS"					; b = IN_ACCESS					; }
    else if(m & IN_ATTRIB)					{ s = "IN_ATTRIB"					; b = IN_ATTRIB					; }
    else if(m & IN_CLOSE_WRITE)			{ s = "IN_CLOSE_WRITE"		; b = IN_CLOSE_WRITE		; }
    else if(m & IN_CLOSE_NOWRITE)		{ s = "IN_CLOSE_NOWRITE"	; b = IN_CLOSE_NOWRITE	; }
    else if(m & IN_CREATE)					{ s = "IN_CREATE"					; b = IN_CREATE					; } // connect
    else if(m & IN_DELETE)					{ s = "IN_DELETE"					; b = IN_DELETE					; } // disintegrate
    else if(m & IN_DELETE_SELF)			{ s = "IN_DELETE_SELF"		; b = IN_DELETE_SELF		; } // disintegrate
    else if(m & IN_MODIFY)					{ s = "IN_MODIFY"					; b = IN_MODIFY					; } // invalidate
    else if(m & IN_MOVE_SELF)				{ s = "IN_MOVE_SELF"			; b = IN_MOVE_SELF			; }
    else if(m & IN_MOVED_FROM)			{ s = "IN_MOVED_FROM"			; b = IN_MOVED_FROM			; }
    else if(m & IN_MOVED_TO)				{ s = "IN_MOVED_TO"				; b = IN_MOVED_TO				; }
    else if(m & IN_OPEN)						{ s = "IN_OPEN"						; b = IN_OPEN						; }
    else if(m & IN_IGNORED)					{ s = "IN_IGNORED"				; b = IN_IGNORED				; }
    else if(m & IN_ISDIR)						{ s = "IN_ISDIR"					; b = IN_ISDIR					; }
    else if(m & IN_Q_OVERFLOW)			{ s = "IN_Q_OVERFLOW"			; b = IN_Q_OVERFLOW			; }
    else if(m & IN_UNMOUNT)					{ s = "IN_UNMOUNT"				; b = IN_UNMOUNT				; }
    else														{ s = "unknown"						; b = ~0								; }

    if(said)
      xsays("|");
    xsays(s);
    said = 1;
    m &= ~b;
  }

  finally : coda;
}
