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

#include <inttypes.h>

#include "narrator.h"
#include "narrator/fixed.h"
#include "narrator/units.h"
#include "fab/ipc.h"
#include "fab/client.h"
#include "fab/metadata.h"
#include "fab/metadata.desc.h"

#include "metadata.h"
#include "params.h"
#include "zbuffer.h"
#include "times.h"
#include "common/attrs.h"

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"usage : fab metadata\n"
"\n"
  );
}

//
// build
//

static xapi connected(command * restrict cmd, fab_client * restrict client)
{
  enter;

  fabipc_message * msg;
  size_t z;

  /* send the request */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;

  z = 0;
  z += znloads(msg->text + z, sizeof(msg->text) - z, "metadata");

  // two terminating null bytes
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  msg->size = z;

  fab_client_post(client, msg);

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  fab_metadata md;
  char buf[128];
  narrator_fixed fixed;
  size_t z;

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    goto XAPI_FINALLY;
  }

  RUNTIME_ASSERT(msg->type == FABIPC_MSG_RESULT);

  z = 0;
  z += descriptor_type_unmarshal(&md, &descriptor_fab_metadata, msg->text, msg->size);
  RUNTIME_ASSERT(z == msg->size);

  printf("%15s : %.*s\n", "project-hash", (int)md.project_hash_len, md.project_hash);
  printf("%15s : %.*s\n", "project-dir", (int)md.project_dir_len, md.project_dir);
  printf("%15s : %.*s\n", "ipc-dir", (int)md.ipcdir_len, md.ipcdir);
  printf("%15s : %.*s\n", "home-dir", (int)md.homedir_len, md.homedir);
  printf("%15s : %ld\n", "fabd-pid", md.fabd_pid);

  fatal(interval_say, md.uptime / NSEC_PER_SEC, narrator_fixed_init(&fixed, buf, sizeof(buf)));
  printf("%15s : %s\n", "uptime", buf);

  z = znload_attrs16(buf, sizeof(buf), bam_global_state_attrs, md.global_state);
  printf("%15s : %.*s\n", "global-state", (int)z, buf);

  printf("%15s : %s\n", "autorun", md.autorun ? "true" : "false");
  printf("%15s :\n%.*s\n", "goals", (int)md.goals_len, md.goals);

  finally : coda;
}

//
// public
//

struct command metadata_command = {
    name : "metadata"
  , usage : usage
  , connected : connected
  , process : process
};
