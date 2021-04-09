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

#include "common/assure.h"
#include "common/snarf.h"
#include "logger/arguments.h"
#include "xlinux/xstdlib.h"
#include "yyutil/parser.h"

#include "args.h"
#include "errtab/MAIN.errtab.h"

#include "macros.h"
#include "zbuffer.h"

/* flex and bison do not agree on these names */
#define YYSTYPE yyu_lval
#define YYLTYPE yyu_location

#include "args.lex.h"
#include "args.states.h"
#include "args.tab.h"
#include "args.tokens.h"

#if DEVEL
#define SYSTEM_CONFIG_PATH    "/etc/bam/client_config+devel"
#define USER_CONFIG_PATH      "$HOME/.bam/client_config+devel"
#else
#define SYSTEM_CONFIG_PATH    "/etc/bam/client_config"
#define USER_CONFIG_PATH      "$HOME/.bam/client_config"
#endif

static YYU_VTABLE(args_vtable, args, args);
typeof(g_args) g_args;
typeof(g_cmd) g_cmd;

//
// public
//

static void args_usage(command * restrict cmd)
{
  printf(
"fab : fab client\n"
"\n"
"usage: fab [command | option]..\n"
"\n"
"commands\n"
" adhoc                 adhoc request\n"
" autobuild             as build, but remain connected\n"
" build       (default) bring targets up-to-date\n"
" events                listen for and print events\n"
" touch                 mark files as out-of-date\n"
" stats                 print stats\n"
" describe              print node info\n"
" tree                  print node trees\n"
" ls                    print node listings\n"
" metadata              print metadata\n"
" config                print effective configuration\n"
" vars                  print effective vars\n"
" reconcile             reload everything\n"
"\n"
"global options\n"
" --help | -h       this message\n"
" --version | -V    print version information\n"
" -B                first, global invalidation\n"
" -K                first, kill extant fabd if any\n"
#if DEVEL
" --no-launch       only talk to an existing fabd\n"
#endif
  );

  if(cmd) {
    printf("\n");
    cmd->usage(cmd);
  }

  printf(
"\n"
"config files\n"
" "SYSTEM_CONFIG_PATH"\n"
" "USER_CONFIG_PATH"\n"
"\n"
"Build optimally. More info at https://github.com/hossbeast/fab\n"
  );

  exit(0);
}

static xapi parse(yyu_parser * restrict parser, char * argvs, int argvsl, const char * restrict fname)
{
  enter;

  fatal(yyu_parse, parser, argvs, argvsl + 2, fname, YYU_INPLACE, 0, 0);

  finally : coda;
}

static xapi get_homedir(const char ** restrict homep)
{
  enter;

  *homep = getenv("HOME");

  finally : coda;
}

static xapi parse_file(yyu_parser * restrict parser, const char * restrict pathspec)
{
  enter;

  char buf[512];
  size_t z;
  const char * path = 0;
  char *text = 0;
  size_t text_len;
  const char *home;
  size_t len;

  len = strlen(pathspec);
  if(len >= 2 && memcmp(pathspec, "~/", 2) == 0)
  {
    fatal(get_homedir, &home);
    z = 0;
    z += znloads(buf + z, sizeof(buf) - z, home);
    z += znloads(buf + z, sizeof(buf) - z, "/");
    z += znloads(buf + z, sizeof(buf) - z, pathspec + 2);
    buf[z] = 0;
    path = buf;
  }
  else if(len >= 6 && memcmp(pathspec, "$HOME/", 6) == 0)
  {
    fatal(get_homedir, &home);
    z = 0;
    z += znloads(buf + z, sizeof(buf) - z, home);
    z += znloads(buf + z, sizeof(buf) - z, "/");
    z += znloads(buf + z, sizeof(buf) - z, pathspec + 6);
    buf[z] = 0;
    path = buf;
  }
  else
  {
    path = pathspec;
  }

  fatal(usnarfs, &text, &text_len, path);
  if(text)
  {
    fatal(parse, parser, text, text_len, path);
  }

finally:
  wfree(text);
coda;
}

xapi args_parse()
{
  enter;

  yyu_parser yyu;
  char *argvs;
  int argvsl;

  g_args.stats = &stats_args;
  g_args.tree = &tree_args;
  g_args.ls = &ls_args;
  g_args.adhoc = &adhoc_args;
  g_args.build = &build_args;
  g_args.touch = &touch_args;
  g_args.describe = &describe_args;
  g_args.events = &events_args;

  memset(&yyu, 0, sizeof(yyu));
  fatal(yyu_parser_init, &yyu, &args_vtable, 0);
  fatal(yyu_parser_init_tokens, &yyu, args_token_table, args_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states, &yyu, args_numstates, args_statenumbers, args_statenames);

  /* parse well-known config files */
  fatal(parse_file, &yyu, SYSTEM_CONFIG_PATH);
  fatal(parse_file, &yyu, USER_CONFIG_PATH);

  /* skate past the program name */
  if((argvs = strchr(g_argvs, ' ')) == 0) {
    g_cmd = &build_command;
    goto XAPI_FINALIZE;
  }

  argvsl = g_argvsl - (argvs - g_argvs);

  /* parse cmd-line args */
  fatal(parse, &yyu, argvs, argvsl, "-args-");

  if(g_args.help || g_args.version)
  {
    args_usage(g_cmd);
  }

  if(!g_cmd) {
    fail(MAIN_NOCOMMAND);
  }

finally:
  fatal(yyu_parser_xdestroy, &yyu);
coda;
}
