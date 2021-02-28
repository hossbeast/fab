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
#include "valyria/pstring.h"
#include "value/writer.h"
#include "logger/arguments.h"
#include "fab/ipc.h"
#include "fab/client.h"
#include "fab/fsent.h"
#include "fab/stats.h"
#include "fab/stats.desc.h"
#include "xlinux/xstdlib.h"

#include "common/attrs.h"

#include "stats.h"
#include "command.h"
#include "MAIN.errtab.h"
#include "params.h"
#include "args.h"

#include "zbuffer.h"
#include "marshal.h"
#include "hexdump.h"

struct stats_args stats_args;
static struct stats_args *args = &stats_args;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"usage : fab stats\n"
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
  int x;

  /* send the request */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;

  z = 0;
  if(args->targets_len == 0)
  {
    if(args->reset) {
      z += znloads(msg->text + z, sizeof(msg->text) - z, "global-stats-reset");
    } else {
      z += znloads(msg->text + z, sizeof(msg->text) - z, "global-stats-read");
    }
  }
  else
  {
    z += znloads(msg->text + z, sizeof(msg->text) - z, "[ ");
    z += znloads(msg->text + z, sizeof(msg->text) - z, "select : [");

    for(x = 0; x < args->targets_len; x++)
    {
      z += znloadf(msg->text + z, sizeof(msg->text) - z, " path : \"%.*s\"", (int)args->targets[x].len, args->targets[x].s);
    }

    z += znloads(msg->text + z, sizeof(msg->text) - z, " ] ");
    if(args->reset) {
      z += znloads(msg->text + z, sizeof(msg->text) - z, "stats-reset");
    } else {
      z += znloads(msg->text + z, sizeof(msg->text) - z, "stats-read");
    }
    z += znloads(msg->text + z, sizeof(msg->text) - z, " ]");
  }

  // two terminating null bytes
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  msg->size = z;

  fab_client_post(client, msg);

  finally : coda;
}

static xapi process_global(fabipc_message * restrict msg)
{
  enter;

  fab_global_stats stats;
  descriptor_field *member;
  int x;
  size_t z;
  void *src;
  size_t sz;

  src = msg->text;
  sz = msg->size;
  z = 0;

  z += descriptor_type_unmarshal(&stats, &descriptor_fab_global_stats, src + z, sz - z);
  RUNTIME_ASSERT(z == msg->size);

  for(x = 0; x < descriptor_fab_global_stats.members_len; x++)
  {
    member = descriptor_fab_global_stats.members[x];

    if(member->size == 8) {
      printf("%30.*s : %"PRIu64"\n", (int)member->name_len, member->name, stats.u64[member->offset / 8]);
    } else if(member->size == 4) {
      printf("%30.*s : %"PRIu32"\n", (int)member->name_len, member->name, stats.u32[member->offset / 4]);
    } else if(member->size == 2) {
      printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats.u16[member->offset / 2]);
    }
  }

  finally : coda;
}

static xapi process_node(fabipc_message * restrict msg)
{
  enter;

  fab_fsent_stats node;
  fab_module_stats mod;
  descriptor_field *member;
  int x;
  size_t z;
  void *src = msg->text;
  size_t sz = msg->size;

  z = 0;

  // base node stats
  z += descriptor_type_unmarshal(&node, &descriptor_fab_fsent_stats, src + z, sz - z);

  printf("%30s : %s\n", "abspath", node.abspath);
  printf("%30s : %s\n", "type", attrs16_name_byvalue(fab_fsent_type_attrs, node.type));
  printf("%30s : %s\n", "state", attrs16_name_byvalue(fab_fsent_state_attrs, node.state));
  for(x = 3; x < descriptor_fab_fsent_stats.members_len; x++)
  {
    member = descriptor_fab_fsent_stats.members[x];
    if(member->size == 8) {
      printf("%30.*s : %"PRIu64"\n", (int)member->name_len, member->name, node.u64[member->offset / 8]);
    } else if(member->size == 4) {
      printf("%30.*s : %"PRIu32"\n", (int)member->name_len, member->name, node.u32[member->offset / 4]);
    } else if(member->size == 2) {
      printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, node.u16[member->offset / 2]);
    } else if(member->size == 1) {
      printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, node.u8[member->offset / 1]);
    }
  }

  /* per node-type stats */
  if(node.type == FAB_FSENT_TYPE_MODULE_DIR)
  {
    z += descriptor_type_unmarshal(&mod, &descriptor_fab_module_stats, src + z, sz - z);

    for(x = 0; x < descriptor_fab_module_stats.members_len; x++)
    {
      member = descriptor_fab_module_stats.members[x];
      if(member->size == 8) {
        printf("%30.*s : %"PRIu64"\n", (int)member->name_len, member->name, mod.u64[member->offset / 8]);
      } else if(member->size == 4) {
        printf("%30.*s : %"PRIu32"\n", (int)member->name_len, member->name, mod.u32[member->offset / 4]);
      } else if(member->size == 2) {
        printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, mod.u16[member->offset / 2]);
      } else if(member->size == 1) {
        printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, mod.u8[member->offset / 1]);
      }
    }
  }
  else if(node.type == FAB_FSENT_TYPE_MODULE_FILE)
  {
    z += descriptor_type_unmarshal(&mod, &descriptor_fab_module_file_stats, src + z, sz - z);

    for(x = 0; x < descriptor_fab_module_file_stats.members_len; x++)
    {
      member = descriptor_fab_module_file_stats.members[x];
      if(member->size == 8) {
        printf("%30.*s : %"PRIu64"\n", (int)member->name_len, member->name, mod.u64[member->offset / 8]);
      } else if(member->size == 4) {
        printf("%30.*s : %"PRIu32"\n", (int)member->name_len, member->name, mod.u32[member->offset / 4]);
      } else if(member->size == 2) {
        printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, mod.u16[member->offset / 2]);
      } else if(member->size == 1) {
        printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, mod.u8[member->offset / 1]);
      }
    }
  }
  else if(node.type == FAB_FSENT_TYPE_FORMULA_FILE)
  {
    z += descriptor_type_unmarshal(&mod, &descriptor_fab_formula_stats, src + z, sz - z);

    for(x = 0; x < descriptor_fab_formula_stats.members_len; x++)
    {
      member = descriptor_fab_formula_stats.members[x];
      if(member->size == 8) {
        printf("%30.*s : %"PRIu64"\n", (int)member->name_len, member->name, mod.u64[member->offset / 8]);
      } else if(member->size == 4) {
        printf("%30.*s : %"PRIu32"\n", (int)member->name_len, member->name, mod.u32[member->offset / 4]);
      } else if(member->size == 2) {
        printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, mod.u16[member->offset / 2]);
      } else if(member->size == 1) {
        printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, mod.u8[member->offset / 1]);
      }
    }
  }
  else if(node.type == FAB_FSENT_TYPE_VAR_FILE)
  {
    z += descriptor_type_unmarshal(&mod, &descriptor_fab_var_stats, src + z, sz - z);

    for(x = 0; x < descriptor_fab_var_stats.members_len; x++)
    {
      member = descriptor_fab_var_stats.members[x];
      if(member->size == 8) {
        printf("%30.*s : %"PRIu64"\n", (int)member->name_len, member->name, mod.u64[member->offset / 8]);
      } else if(member->size == 4) {
        printf("%30.*s : %"PRIu32"\n", (int)member->name_len, member->name, mod.u32[member->offset / 4]);
      } else if(member->size == 2) {
        printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, mod.u16[member->offset / 2]);
      } else if(member->size == 1) {
        printf("%30.*s : %"PRIu16"\n", (int)member->name_len, member->name, mod.u8[member->offset / 1]);
      }
    }
  }

  RUNTIME_ASSERT(z == msg->size);

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    goto XAPI_FINALLY;
  }

  RUNTIME_ASSERT(msg->type == FABIPC_MSG_RESULT);

  if(args->targets_len == 0) {
    fatal(process_global, msg);
  } else {
    fatal(process_node, msg);
  }

  finally : coda;
}

//
// public
//

struct command stats_command = {
    usage : usage
  , connected : connected
  , process : process
};

#if 0
xapi stats_command_cleanup()
{
  enter;

  fatal(list_xfree, args.targets);

  finally : coda;
}

static xapi parse_args(command * restrict cmd, int argc, char ** restrict argv)
{
  enter;

  int x;
  int longindex;
  pstring s = { };

  const struct option longopts[] = {
      { }
  };

  const char *switches =
    // no-argument switches
    "r"

    // with-argument switches
    ""
  ;

  fatal(list_createx, &args.targets, 0, 0, wfree, 0);

  // disable getopt error messages
  opterr = 0;
  optind = 0;
  while((x = getopt_long(argc, argv, switches, longopts, &longindex)) != -1)
  {
    if(x == 0)
    {
      // longopts
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
    else if(x == 'r')
    {
      reset = true;
    }
  }

  while(optind < argc)
  {
    fatal(psinit, &s);
    fatal(pscats, &s, argv[optind++]);
    fatal(list_push, args.targets, s.s, 0);
    memset(&s, 0, sizeof(s));
  }

finally:
  fatal(psdestroy, &s);
coda;
}
#endif
