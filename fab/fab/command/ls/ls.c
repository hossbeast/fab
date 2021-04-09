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

#include "ls.h"
#include "command.h"
#include "params.h"
#include "zbuffer.h"

static uint64_t requestid;
struct ls_args ls_args = {
    graph : FAB_GRAPH_FSTREE
  , direction : "down"
};
static struct ls_args *args = &ls_args;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"ls options\n"
" -g <graph-type>           show this graph\n"
" --down                    downwards from the subject node\n"
" --up                      upwards from the subject node\n"
" --paths                   show full paths\n"
"\n"
"graph types\n"
" fstree          (default) filesystem tree\n"
" dirtree                   directories subset of filesystem tree\n"
" modtree                   modules subset of filesystem tree\n"
" depends                   dependency graph\n"
" imports                   module imports graph\n"
" uses                      module uses graph\n"
" requires                  module requires graph\n"
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
  char *dst;
  size_t sz;
  bead target;

  target = args->target;
  if(target.len == 0) {
    target = (bead){ ".", 1 };
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
  z += znloadf(dst + z, sz - z, "    direction : %s", args->direction);
  z += znloads(dst + z, sz - z, "    order : preorder");
  z += znloadf(dst + z, sz - z, "    graph : %s", attrs16_name_byvalue(fab_graph_attrs, args->graph));
  z += znloads(dst + z, sz - z, "    min-distance : 1");
  z += znloads(dst + z, sz - z, "    max-distance : 1");
  z += znloads(dst + z, sz - z, "   }");
  z += znloads(dst + z, sz - z, "  ]");
  z += znloads(dst + z, sz - z, "  list");
  z += znloads(dst + z, sz - z, " ]");
  z += znloadc(dst + z, sz - z, 0);
  z += znloadc(dst + z, sz - z, 0);

  msg->size = z;
  client_post(client, msg);

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  int x;
  size_t z;
  fab_list_item item;

  RUNTIME_ASSERT(msg->id == requestid);

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    goto XAPI_FINALLY;
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

  if(ls_args.show_paths) {
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

  finally : coda;
}

//
// public
//

struct command ls_command = {
    usage : usage
  , connected : connected
  , process : process
};
