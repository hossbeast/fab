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
#include <string.h>
#include <getopt.h>

#include "xapi.h"
#include "types.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "logger/arguments.h"

#include "xapi/SYS.errtab.h"
#include "args.h"
#include "common/parseint.h"
#include "common/hash.h"
#include "filesystem.h"
#include "common/attrs.h"

struct g_args_t g_args;

#if DEVEL
#define SYSTEM_CONFIG_PATH    "/etc/fabconfig+devel"
#define USER_CONFIG_PATH      "$HOME/.fab/config+devel"
#else
#define SYSTEM_CONFIG_PATH    "/etc/fabconfig"
#define USER_CONFIG_PATH      "$HOME/.fab/config"
#endif

#define DEFAULT_SWEEPER_PERIOD MSEC_AS_NSEC(250)

#if DEVEL
static xapi args_parse()
{
  enter;

  char *pwd = 0;
  int x;
  int longindex;

  const struct option longopts[] = {
      { "system-config-path"            , required_argument, 0, 0 }
    , { "user-config-path"              , required_argument, 0, 0 }
    , { "default-filesystem-invalidate" , required_argument, 0, 0 }
    , { "sweeper-period-nsec"           , required_argument, 0, 0 }
    , { }
  };

  const char *switches = "K";

  g_args.default_filesystem_invalidate = FILESYSTEM_INVALIDATE_DEFAULT;

  // disable getopt error messages
  opterr = 0;
  optind = 0;
  while((x = getopt_long(g_argc, g_argv, switches, longopts, &longindex)) != -1)
  {
    if(x == 0 && strcmp(longopts[longindex].name, "system-config-path") == 0) {
      iwfree(&g_args.system_config_path);
      fatal(ixstrdup, &g_args.system_config_path, optarg);
    } else if(x == 0 && strcmp(longopts[longindex].name, "user-config-path") == 0) {
      iwfree(&g_args.user_config_path);
      fatal(ixstrdup, &g_args.user_config_path, optarg);
    } else if(x == 0 && strcmp(longopts[longindex].name, "default-filesystem-invalidate") == 0) {
      if((g_args.default_filesystem_invalidate = attrs16_value_byname(invalidate_attrs, optarg)) == 0) {
        fail(SYS_BADARGS);
      }
    } else if(x == 0 && strcmp(longopts[longindex].name, "sweeper-period-nsec") == 0) {
      if(parseuint(optarg, SCNu64, 1, UINT64_MAX, 1, 20, &g_args.sweeper_period_nsec, 0) != 0) {
        fail(SYS_BADARGS);
      }
    } else if(x == 'K') {
      g_args.kill = true;
    }
  }

  if(optind == g_argc) {
    fatal(xrealpaths, &pwd, 0, getenv("PWD"));
    g_args.hash = hash64(0, pwd, strlen(pwd));
  }
  if(optind < g_argc) {
    if(parseuint(g_argv[optind++], SCNx64, 1, UINT64_MAX, 1, UINT8_MAX, &g_args.hash, 0) != 0) {
      fail(SYS_BADARGS);
    }
  }
  if(optind < g_argc) {
    fails(SYS_BADARGS, "unknown arg", g_argv[optind]);
  }

finally:
  free(pwd);
coda;
}
#else
static xapi args_parse()
{
  enter;

  /* single, required argument */
  if(g_argc < 2 || parseuint(g_argv[1], SCNx64, 1, UINT64_MAX, 1, UINT8_MAX, &g_args.hash, 0) != 0) {
    fail(SYS_BADARGS);
  }

  finally : coda;
}
#endif

xapi args_setup()
{
  enter;

  char space[64];

  /* defaults */
  fatal(ixstrdup, &g_args.system_config_path, SYSTEM_CONFIG_PATH);
  fatal(ixstrdup, &g_args.user_config_path, USER_CONFIG_PATH);

  fatal(args_parse);
  snprintf(space, sizeof(space), "%016"PRIx64, g_args.hash);
  fatal(ixstrdup, &g_args.hash_str, space);

  finally : coda;
}

xapi args_teardown()
{
  enter;

  wfree(g_args.system_config_path);
  wfree(g_args.user_config_path);

  finally : coda;
}
