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

#include "common/attrs.h"
#include "fab/client.h"
#include "fab/describe.h"
#include "fab/fsent.h"
#include "fab/ipc.h"

#include "describe.h"
#include "args.h"
#include "command.h"
#include "params.h"

#include "zbuffer.h"

struct describe_args describe_args;
static struct describe_args *args = &describe_args;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"describe [<path>...]\n"
  );
}

static xapi connected(command * restrict cmd, fab_client * restrict client)
{
  enter;

  fabipc_message * msg;
  size_t z;
  int x;

  /* send the request */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;

  z = 0;
  z += znloads(msg->text + z, sizeof(msg->text) - z, "[ ");
  z += znloads(msg->text + z, sizeof(msg->text) - z, "select : [");

  for(x = 0; x < args->targets_len; x++)
  {
    z += znloadf(msg->text + z, sizeof(msg->text) - z, " path : \"%.*s\"", (int)args->targets[x].len, args->targets[x].s);
  }

  z += znloads(msg->text + z, sizeof(msg->text) - z, " ] ");
  z += znloads(msg->text + z, sizeof(msg->text) - z, "describe");
  z += znloads(msg->text + z, sizeof(msg->text) - z, " ]");

  // two terminating null bytes
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  msg->size = z;

  client_post(client, msg);

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  fab_describe_item node;
  int x;
  descriptor_field *member;
  const char *str;
  uint16_t len;

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    goto XAPI_FINALLY;
  }

  RUNTIME_ASSERT(msg->type == FABIPC_MSG_RESULT);

  descriptor_type_unmarshal(&node, &descriptor_fab_describe_item, msg->text, msg->size);

  printf("%.*s : {\n", (int)node.abspath_len, node.abspath);
  printf(" %s : %s\n", "type", attrs16_name_byvalue(fab_fsent_type_attrs, node.type));
  printf(" %s : %s\n", "state", attrs16_name_byvalue(fab_fsent_state_attrs, node.state));

  for(x = 3; x < descriptor_fab_describe_item.members_len; x++)
  {
    member = descriptor_fab_describe_item.members[x];
    str = node.s[member->offset / sizeof(str)];
    len = node.u16[member->len_offset / sizeof(len)];
    printf(" %.*s : %.*s\n", (int)member->name_len, member->name, (int)len, str);
  }
  printf("}\n");

  finally : coda;
}

//
// public
//

struct command describe_command = {
    usage : usage
  , connected : connected
  , process : process
};
