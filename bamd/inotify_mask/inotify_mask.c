/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/inotify.h>

#include "narrator.h"

#include "inotify_mask.h"
#include "zcat.h"

void inotify_mask_say(uint32_t m, struct narrator * N)
{
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
}

size_t inotify_mask_cat(char * restrict dst, size_t sz, uint32_t m)
{
  size_t z = 0;

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
      z += zcatc(dst + z, sz - z, '|');
    z += zcats(dst + z, sz - z, s);
    said = 1;
    m &= ~b;
  }

  return z;
}
