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
#include "common/color.h"
#include "fab/client.h"
#include "fab/ipc.h"
#include "fab/list.h"

#include "tree.h"
#include "command.h"
#include "params.h"
#include "zbuffer.h"

struct tree_args tree_args = {
    graph : FAB_GRAPH_FSTREE
  , direction : "down"
};
static struct tree_args *args = &tree_args;
static bool upwards;
static bool second;
static uint64_t requestid;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"tree options\n"
  );
}

//
// build
//

static void post_request(fab_client * restrict client)
{
  fabipc_message * msg;
  size_t z;
  char *dst;
  size_t sz;
  bead target;
  const char *dir;
  const char *order;
  const char *dist;

  target = args->target;
  if(target.len == 0) {
    target = (bead){ ".", 1 };
  }

  dir = "down";
  order = "preorder";
  if(upwards) {
    dir = "up";
    order = "postorder";
  }

  dist = "0";
  if(second) {
    dist = "1";
  }

  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  dst = msg->text;
  sz = sizeof(msg->text);
  z = 0;
  z += znloads(dst + z, sz - z, " [");
  z += znloads(dst + z, sz - z, "  select : [");
  z += znloadf(dst + z, sz - z, "   path : \"%.*s\"", (int)target.len, target.s);
  z += znloads(dst + z, sz - z, "   traverse : {");
  z += znloadf(dst + z, sz - z, "    direction : %s", dir);
  z += znloadf(dst + z, sz - z, "    order : %s", order);
  z += znloadf(dst + z, sz - z, "    graph : %s", attrs16_name_byvalue(fab_graph_attrs, args->graph));
  z += znloadf(dst + z, sz - z, "    min-distance : %s", dist);
  z += znloads(dst + z, sz - z, "   }");
  z += znloads(dst + z, sz - z, "  ]");
  z += znloads(dst + z, sz - z, "  list");
  z += znloads(dst + z, sz - z, " ]");
  z += znloadc(dst + z, sz - z, 0);
  z += znloadc(dst + z, sz - z, 0);

  msg->size = z;
  client_post(client, msg);
}

static void connected(command * restrict cmd, fab_client * restrict client)
{
  upwards = false;
  if(args->both) {
    upwards = true;
  } else if(strcmp(args->direction, "up") == 0) {
    upwards = true;
  }

  post_request(client);
}

static void process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  int x;
  size_t z;
  fab_list_item item;

  RUNTIME_ASSERT(msg->id == requestid);

  if(msg->type == FABIPC_MSG_RESPONSE && args->both && !second) {
    upwards = false;
    second = true;
    post_request(client);
    return;
  }

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    return;
  }

  RUNTIME_ASSERT(msg->type == FABIPC_MSG_RESULT);

  z = 0;
  z += descriptor_type_unmarshal(&item, &descriptor_fab_list_item, msg->text, msg->size);
  RUNTIME_ASSERT(z == msg->size);

  for(x = 0; x < item.distance; x++) {
    dprintf(1, "|-- ");
  }

  if(item.type == FAB_FSENT_TYPE_MODULE_DIR) {
    write(1, YELLOW);
  } else if(item.type == FAB_FSENT_TYPE_DIR) {
    write(1, BLUE);
  } else if(item.type == FAB_FSENT_TYPE_SHADOW_DIR) {
    write(1, GREEN);
  }

  if(tree_args.show_paths) {
    dprintf(1, "%.*s", item.path_len, item.path);
  } else {
    dprintf(1, "%.*s", item.label_len, item.label);
  }

  dprintf(1, " - %s %s"
    , attrs16_name_byvalue(fab_fsent_type_attrs, item.type)
    , attrs16_name_byvalue(fab_fsent_state_attrs, item.state)
  );
  if(item.ref_len) {
    dprintf(1, " -> %.*s", item.ref_len, item.ref);
  }

  if(item.type == FAB_FSENT_TYPE_DIR || item.type == FAB_FSENT_TYPE_MODULE_DIR || item.type == FAB_FSENT_TYPE_SHADOW_DIR) {
    write(1, NOCOLOR);
  }

  dprintf(1, "\n");
}

//
// public
//

struct command tree_command = {
    usage : usage
  , connected : connected
  , process : process
};
