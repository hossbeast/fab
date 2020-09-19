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

#include "types.h"
#include "xapi.h"

#include "fab/ipc.h"
#include "valyria/llist.h"
#include "xlinux/xshm.h"
#include "xlinux/xfutex.h"

#include "channel.h"
#include "config.internal.h"
#include "params.h"

static llist freelist = LLIST_INITIALIZER(freelist);

/* ugo+rw  world read/write */
#define SHMCHANNEL_MODE 0666

//
// public
//

xapi channel_create(fabipc_channel ** restrict chanp, pid_t thread_id)
{
  enter;

  fabipc_channel *chan;
  int shmid = -1;
  void * shmaddr = 0;

  fatal(xshmget, IPC_PRIVATE, FABIPC_SHMSIZE, IPC_CREAT | IPC_EXCL | SHMCHANNEL_MODE, &shmid);
  fatal(xshmat, shmid, 0, 0, &shmaddr);

  chan = shmaddr;
  chan->shmid = shmid;
  chan->server_pid = g_params.pid;
  chan->server_tid = thread_id;
  chan->server_pulse = thread_id;
  *chanp = chan;

finally:
  if(shmid != -1) {
    fatal(xshmctl, shmid, IPC_RMID, 0);
  }
coda;
}

xapi channel_xfree(fabipc_channel * restrict chan)
{
  enter;

  if(chan)
  {
    fatal(xshmdt, chan);
  }

  finally : coda;
}

xapi channel_setup()
{
  enter;

  finally : coda;
}

xapi channel_cleanup()
{
  enter;

  finally : coda;
}

xapi channel_reconfigure(config * restrict cfg, bool dry)
{
  enter;

  if(dry)
    goto XAPI_FINALIZE;

  if(!(cfg->channels.attrs & CONFIG_CHANGED))
    goto XAPI_FINALIZE;

  finally : coda;
}
