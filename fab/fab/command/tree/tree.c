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

#include <getopt.h>
#include <inttypes.h>

#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/list.h"
#include "value/writer.h"
#include "logger/arguments.h"
#include "fab/ipc.h"
#include "fab/client.h"
#include "fab/list.h"
#include "fab/graph.h"
#include "xlinux/xstdlib.h"
#include "yyutil/parser.h"

#include "common/attrs.h"
#include "common/color.h"

#include "tree.h"
#include "command.h"
#include "MAIN.errtab.h"
#include "params.h"
#include "args.h"

#include "zbuffer.h"
#include "marshal.h"

struct tree_args tree_args = {
    graph : FAB_GRAPH_FSTREE
  , direction : "down"
};
static struct tree_args *args = &tree_args;
static bool upwards;
static bool second;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"usage : fab tree\n"
"\n"
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
  fab_client_post(client, msg);
}

static xapi connected(command * restrict cmd, fab_client * restrict client)
{
  enter;

  upwards = false;
  if(args->both) {
    upwards = true;
  } else if(strcmp(args->direction, "up") == 0) {
    upwards = true;
  }

  post_request(client);

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  int x;
  size_t z;
  fab_list_item item;

  if(msg->type == FABIPC_MSG_RESPONSE && args->both && !second) {
    upwards = false;
    second = true;
    post_request(client);
    goto XAPI_FINALLY;
  }

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

  if(tree_args.show_paths) {
    dprintf(1, "%.*s", item.abspath_len, item.abspath);
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

struct command tree_command = {
    usage : usage
  , connected : connected
  , process : process
};

#if 0
{
  enter;

  int x;
  int longindex;

  const struct option longopts[] = {
      { "graph", required_argument, 0, 0, }
    , { "up", no_argument, 0, 'u', }
    , { "down", no_argument, 0, 'd', }
    , { "both", no_argument, 0, 'b', }
    , { }
  };

  const char *switches =
    // no-argument switches
    "udb"

    // with-argument switches
    ""
  ;

  // disable getopt error messages
  opterr = 0;
  optind = 0;
  while((x = getopt_long(argc, argv, switches, longopts, &longindex)) != -1)
  {
printf("x %d\n", x);
    if(x == 0)
    {
      if(strcmp(longopts[longindex].name, "graph") == 0) {
        printf("optarg %s\n", optarg);
      }
    }
    else if(x == 'u')
    {
      direction = "up";
      both = false;
    }
    else if(x == 'd')
    {
      direction = "down";
      both = false;
    }
    else if(x == 'b')
    {
      both = true;
    }
    else if(x == 'g')
    {
int i;
for(i = 0; i < argc; i++) {
  printf("%2d %s", i, argv[i]);
  if(optind == i) {
    printf(" *");
  }
  printf("\n");
}
printf("optarg %s\n", optarg);
      if((graph = attrs16_value_byname(fab_graph_attrs, argv[optind])) == 0) {
        fails(MAIN_BADARGS, "unknown graph", argv[optind]);
      }
    }
    else if(x == '?')
    {
      if(optopt)
      {
        failf(MAIN_BADARGS, "unknown switch", "-%c", optopt);
      }
      else
      {
        fails(MAIN_BADARGS, "unknown argument", argv[optind-1]);
      }
    }
  }

  if(optind != (argc - 1))
  {
    fprintf(stderr, "expected exactly one path");
    exit(1);
  }

  target = argv[optind];

  finally : coda;
}

static xapi parse_args(command * restrict cmd, int argc, char ** restrict argv)
{
  enter;

  args_parser parser;
  char *argvs;
  int argvsl;

  args.graph = FAB_GRAPH_FSTREE;
  args.direction = "down";

  memset(&parser, 0, sizeof(parser));
  fatal(yyu_parser_init, &parser.yyu, &tree_args_vtable, 0);
  fatal(yyu_parser_init_tokens, &parser.yyu, tree_args_token_table, tree_args_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states, &parser.yyu, args_numstates, args_statenumbers, args_statenames);

  argvs = strchr(g_argvs, ' ');
  argvsl = g_argvsl - (argvs - g_argvs);

printf("> %.*s\n", argvsl, argvs);

  parser.args = &args;
  fatal(yyu_parse, &parser.yyu, argvs, argvsl + 2, "-args-", YYU_INPLACE, 0, 0);

printf("target %s\n", args.target);
printf("graph %d\n", args.graph);
printf("direction %s\n", args.direction);
printf("both %d\n", args.both);

finally:
  fatal(yyu_parser_xdestroy, &parser.yyu);
coda;
}
#endif
