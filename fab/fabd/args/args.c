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
#include "git-state.h"

struct g_args_t g_args;

#if DEVEL
#define SYSTEM_CONFIG_PATH    "/etc/fabconfig+devel"
#define USER_CONFIG_PATH      "$HOME/.fab/config+devel"
#else
#define SYSTEM_CONFIG_PATH    "/etc/fabconfig"
#define USER_CONFIG_PATH      "$HOME/.fab/config"
#endif

#define DEFAULT_SWEEPER_PERIOD MSEC_AS_NSEC(250)

static void args_version()
{
  printf("fabd - fab daemon %s\n\n%s\n", git_describe, git_metadata);

  exit(0);
}

static void args_usage()
{
  printf(
"fabd - fab daemon %s\n"
"\n"
"usage: fabd [option] <hash>\n"
"\n"
"options\n"
" --help | -h       print this message\n"
" --version | -V    print version information\n"
"\n"
"build optimally      https://github.com/hossbeast/fab\n"
    , git_describe
  );

  exit(0);
}

static xapi args_parse()
{
  enter;

  char *pwd = 0;
  int x;
  int longindex;
  int version = 0;
  int help = 0;

  const struct option longopts[] = {
      { "help"     , no_argument , &help, 1 }
    , { "version"  , no_argument , &version, 1 }
#if DEVEL
    , { "system-config-path"            , required_argument, 0, 0 }
    , { "user-config-path"              , required_argument, 0, 0 }
    , { "default-filesystem-invalidate" , required_argument, 0, 0 }
    , { "sweeper-period-nsec"           , required_argument, 0, 0 }
#endif
    , { }
  };

  const char *switches = "hV"
#if DEVEL
    "K"
#endif
  ;

  g_args.default_filesystem_invalidate = FILESYSTEM_INVALIDATE_DEFAULT;
  fatal(ixstrdup, &g_args.system_config_path, SYSTEM_CONFIG_PATH);
  fatal(ixstrdup, &g_args.user_config_path, USER_CONFIG_PATH);

  // disable getopt error messages
  opterr = 0;
  optind = 0;
  while((x = getopt_long(g_argc, g_argv, switches, longopts, &longindex)) != -1)
  {
#if DEVEL
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
#endif
    if(x == 'h') {
      help = 1;
    } else if(x == 'V') {
      version = 1;
    }
  }

  if(help)
  {
    args_usage();
  }
  if(version)
  {
    args_version();
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

xapi args_setup()
{
  enter;

  char space[64];

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
