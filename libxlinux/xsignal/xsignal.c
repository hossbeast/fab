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

#include <signal.h>

#include "internal.h"

API xapi xkill(pid_t pid, int sig)
{
  enter;

	fatalize(errno, kill, pid, sig);

finally:
	XAPI_INFOF("pid", "%ld", (long)pid);
	XAPI_INFOF("sig", "%d", sig);
coda;
}

API xapi uxkill(pid_t pid, int sig, int * r)
{
  enter;

	if(r && ((*r) = kill(pid, sig)) == -1 && errno != ESRCH)
	{
		fail(errno);
	}
	else if(!r && kill(pid, sig) == -1 && errno != ESRCH)
	{
		fail(errno);
	}

	finally : coda;
}

API xapi xsignal(int signum, sighandler_t handler)
{
  enter;

	fatalize(errno, signal, signum, handler);

finally:
	XAPI_INFOF("sig", "%d", signum);
coda;
}

API xapi xsigaction(int signum, const struct sigaction * act, struct sigaction * oldact)
{
  enter;

	fatalize(errno, sigaction, signum, act, oldact);

finally:
	XAPI_INFOF("sig", "%d", signum);
coda;
}

API xapi xsigprocmask(int how, const sigset_t * set, sigset_t * oldset)
{
  enter;

	fatalize(errno, sigprocmask, how, set, oldset);

	finally : coda;
}

API xapi uxsigsuspend(const sigset_t * mask)
{
  enter;

  if(sigsuspend(mask))
  {
    if(errno == EINTR)
    {
      /* the whole reason to use this function ... */
    }
    else
    {
      fail(errno);
    }
  }

  finally : coda;
}
