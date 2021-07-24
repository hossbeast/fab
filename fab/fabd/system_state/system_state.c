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

//
//#include "fab/sigutil.h"
//#include "xlinux/xpthread.h"
//
#include "fab/metadata.h"
#include "fab/ipc.h"
#include "fab/events.h"
#include "channel.h"
#include "common/attrs.h"
//#include "config.h"
//#include "extern.h"
//#include "formula.h"
//#include "fsent.h"
//#include "handler.h"
//#include "module.h"
//#include "params.h"
//#include "rule_system.h"
//#include "stats.h"
//#include "var.h"
//#include "walker.h"
//
//#include "atomics.h"

#include "system_state.h"
#include "events.h"
#include "zbuffer.h"

enum bam_system_state system_state = BAM_SYSTEM_STATE_BOOTSTRAP;
bool system_error;

void system_state_change(bam_system_state state)
{
  channel *chan;
  fabipc_message *msg;

  if(state == system_state) {
    return;
  }

  if(!events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
    goto end;
  }

  msg->size = znloadf(msg->text, sizeof(msg->text), "%s -> %s"
    , attrs16_name_byvalue(bam_system_state_attrs, system_state)
    , attrs16_name_byvalue(bam_system_state_attrs, state)
  );
  events_publish(chan, msg);

end:
  system_state = state;
}
