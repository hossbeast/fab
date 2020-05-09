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

#include <sys/epoll.h>

#include "narrator.h"

#include "epoll_attrs.h"

xapi epoll_events_say(uint32_t m, struct narrator * N)
{
  enter;

  int said = 0;
  while(m)
  {
    char* s = 0;
    uint32_t b = 0;

         if(m & EPOLLIN)					{ s = "EPOLLIN"					; b = EPOLLIN; }
    else if(m & EPOLLOUT)					{ s = "EPOLLOUT"				; b = EPOLLOUT; }
    else if(m & EPOLLRDHUP)		  	{ s = "EPOLLRDHUP"		  ; b = EPOLLRDHUP; }
    else if(m & EPOLLPRI)     		{ s = "EPOLLPRI"	      ; b = EPOLLPRI; }
    else if(m & EPOLLERR)					{ s = "EPOLLERR"				; b = EPOLLERR; }
    else if(m & EPOLLHUP)					{ s = "EPOLLHUP"				; b = EPOLLHUP; }
    else if(m & EPOLLET)			    { s = "EPOLLET"		      ; b = EPOLLET; }
    else if(m & EPOLLONESHOT)			{ s = "EPOLLONESHOT"		; b = EPOLLONESHOT; }
    else if(m & EPOLLWAKEUP)			{ s = "EPOLLWAKEUP"			; b = EPOLLWAKEUP; }
    else if(m & EPOLLEXCLUSIVE)		{ s = "EPOLLEXCLUSIVE"	; b = EPOLLEXCLUSIVE; }
    else													{ s = "unknown"					; b = ~0; }

    if(said)
      xsays("|");
    xsays(s);
    said = 1;
    m &= ~b;
  }

  finally : coda;
}
