/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "common/attrs.h"
#include "bam/client.h"
#include "bam/ipc.h"
#include "bam/metadata.desc.h"
#include "bam/metadata.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "narrator/units.h"

#include "metadata.h"
#include "params.h"
#include "zcat.h"
#include "times.h"

static uint64_t requestid;

static void usage(command * restrict cmd)
{
  printf(
"metadata options (none)\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  size_t z;

  /* send the request */
  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  z = 0;
  z += zcats(msg->text + z, sizeof(msg->text) - z, "metadata");

  // two terminating null bytes
  z += zcatc(msg->text + z, sizeof(msg->text) - z, 0);
  z += zcatc(msg->text + z, sizeof(msg->text) - z, 0);
  msg->size = z;

  client_post(client, msg);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  bam_metadata md;
  char buf[128];
  narrator_fixed fixed;
  size_t z;

  RUNTIME_ASSERT(msg->id == requestid);

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    return;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);

  z = 0;
  z += descriptor_type_unmarshal(&md, &descriptor_bam_metadata, msg->text, msg->size);
  RUNTIME_ASSERT(z == msg->size);

  printf("%.*s : {\n", (int)md.base_path_len, md.base_path);
  printf(" %s : %.*s\n", "hash", (int)md.hash_len, md.hash);
  printf(" %s : %.*s\n", "base-path", (int)md.base_path_len, md.base_path);
  printf(" %s : %.*s\n", "ipc-dir", (int)md.ipcdir_len, md.ipcdir);
  printf(" %s : %.*s\n", "home-dir", (int)md.homedir_len, md.homedir);
  printf(" %s : %ld\n", "bamd pid", md.bamd_pid);

  interval_say(md.uptime / NSEC_PER_SEC, narrator_fixed_init(&fixed, buf, sizeof(buf)));
  printf(" %s : \"%s\"\n", "uptime", buf);

  interval_say(md.timeout / NSEC_PER_SEC, narrator_fixed_init(&fixed, buf, sizeof(buf)));
  printf(" %s : \"%s\"\n", "timeout", buf);

  z = attrs16_zcat(buf, sizeof(buf), bam_system_state_attrs, md.system_state);
  printf(" %s : %.*s\n", "system-state", (int)z, buf);
  printf(" %s : %s\n", "system-error", md.system_error ? "yes" : "no");

  printf(" %s : %s\n", "autorun", md.autorun ? "true" : "false");
  printf(" %s :\n%.*s\n", "goals", (int)md.goals_len, md.goals);
  printf("}\n");
}

static void attached(command * restrict cmd, bam_client * restrict client)
{

}

//
// public
//

static void disconnected(command * restrict cmd) { }
struct command metadata_command = {
    name : "metadata"
  , attached : attached
  , usage : usage
  , connected : connected
  , process : process
  , disconnected : disconnected
};
