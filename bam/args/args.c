/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/assure.h"
#include "common/snarf.h"
#include "common/hash.h"
#include "xlinux/xstdlib.h"
#include "yyutil/parser.h"

#include "args.h"
#include "params.h"

#include "macros.h"
#include "zcat.h"

/* flex and bison do not agree on these names */
#define YYSTYPE yyu_lval
#define YYLTYPE yyu_location

#include "args.lex.h"
#include "args.states.h"
#include "args.tab.h"
#include "args.tokens.h"

#if CONFIG_DEVEL
#define SYSTEM_CONFIG_PATH    "/etc/bam/client_config+devel"
#define USER_CONFIG_PATH      "$HOME/.bam/client_config+devel"
#else
#define SYSTEM_CONFIG_PATH    "/etc/bam/client_config"
#define USER_CONFIG_PATH      "$HOME/.bam/client_config"
#endif

#include "git-state.h"

static YYU_VTABLE(args_vtable, args, args);
typeof(g_args) g_args;
typeof(g_cmd) g_cmd;

static char *argvs;
static size_t argvsa;
static size_t argvsl;

//
// public
//

static void version()
{
  printf("bam %s\n\n%s\n", git_describe, git_metadata);

  exit(0);
}

static void usage(command * restrict cmd)
{
  printf(
"bam %s\n"
"\n"
"usage: bam [command | option]..\n"
"\n"
"commands\n"
" adhoc                 adhoc request\n"
" autobuild             as build, but remain connected\n"
" build       (default) carry out the build, mark files fresh\n"
" config                print effective config\n"
" describe              print node info\n"
" env                   print effective env\n"
" events                subscribe to and print events\n"
" filesystem            show filesystem mount points\n"
" fstree                show fstree attachment points\n"
" kill                  terminate an extant bamd if any\n"
" ls                    print directory node listings\n"
" metadata              print metadata\n"
" reconcile             reload everything\n"
" rules                 print effective rules\n"
" stats                 print global, or per-node counters\n"
" touch                 mark files stale\n"
" trace                 wip\n"
" tree                  print directory tree listings\n"
" vars                  print effective vars\n"
"\n"
"global options\n"
" --help | -h       print this message\n"
" --version | -V    print version information\n"
" -B                first, global invalidation\n"
" -K                first, kill extant bamd if any\n"
" -m path/to/module set the context module\n"
#if CONFIG_DEVEL
" --no-launch       only talk to an existing bamd\n"
#endif
    , git_describe
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
"build optimally      https://git.sr.ht/~hossbeast/bam\n"
  );

  exit(0);
}

static int parse(yyu_parser * restrict parser, char * argvs, int argvsl, const char * restrict fname)
{
  int r;

  if(argvsl == 0) {
    return 0;
  }

  if((r = yyu_parse(parser, argvs, argvsl + 2, fname, YYU_INPLACE, 0, 0))) {
    fprintf(stderr, "%.*s\n", parser->error_len, parser->error_str);
  }

  return r;
}

static void get_homedir(const char ** restrict homep)
{
  *homep = getenv("HOME");
}

static int parse_file(yyu_parser * restrict parser, const char * restrict pathspec)
{
  char buf[512];
  size_t z;
  const char * path = 0;
  char *text = 0;
  size_t text_len = 0;
  const char *home;
  size_t len;
  int r;

  len = strlen(pathspec);
  if(len >= 2 && memcmp(pathspec, "~/", 2) == 0)
  {
    get_homedir(&home);
    z = 0;
    z += zcats(buf + z, sizeof(buf) - z, home);
    z += zcats(buf + z, sizeof(buf) - z, "/");
    z += zcats(buf + z, sizeof(buf) - z, pathspec + 2);
    buf[z] = 0;
    path = buf;
  }
  else if(len >= 6 && memcmp(pathspec, "$HOME/", 6) == 0)
  {
    get_homedir(&home);
    z = 0;
    z += zcats(buf + z, sizeof(buf) - z, home);
    z += zcats(buf + z, sizeof(buf) - z, "/");
    z += zcats(buf + z, sizeof(buf) - z, pathspec + 6);
    buf[z] = 0;
    path = buf;
  }
  else
  {
    path = pathspec;
  }

  usnarfs(&text, &text_len, path);
  r = parse(parser, text, text_len, path);
  free(text);
  return r;
}

void args_cleanup()
{
  free(argvs);
}

void args_parse(int argc, char ** argv)
{
  yyu_parser yyu;
  size_t len;
  int x;
  int r;

  /* defaults */
  g_cmd = &build_command;

  memset(&yyu, 0, sizeof(yyu));
  yyu_parser_init(&yyu, &args_vtable);
  yyu_parser_init_tokens(&yyu, args_token_table, args_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(&yyu, args_numstates, args_statenumbers, args_statenames);

  /* parse well-known config files */
  if(parse_file(&yyu, SYSTEM_CONFIG_PATH) != 0) {
    exit(1);
  }
  if(parse_file(&yyu, USER_CONFIG_PATH) != 0) {
    exit(1);
  }

  /* join arguments-list into a single space-delimited string */
  for(x = 1; x < argc; x++)
  {
    len = strlen(argv[x]);
    assure(&argvs, 1, argvsl + len + 3, &argvsa);
    memcpy(argvs + argvsl, argv[x], len);
    memcpy(argvs + argvsl + len, " ", 1);
    argvsl += len + 1;
  }

  if((r = parse(&yyu, argvs, argvsl, "-args-")))
  {
    exit(1);  // error printed earlier
  }
  else if(g_args.help)
  {
    usage(g_cmd);
  }
  else if(g_args.version)
  {
    version();
  }

  RUNTIME_ASSERT(g_cmd);
  yyu_parser_xdestroy(&yyu);

  if((g_cmd->attrs & COMMAND_CONTEXT_MODULE)) {
    if(!g_args.context_module_id) {
      if(!g_args.no_context_module) {
        args_set_module_abspath(g_params.cwd, g_params.cwd_len);
      }
    }
  }
}

void args_set_module_path(const char * restrict path, uint16_t path_len)
{
  char inpath[512];
  char abspath[512];
  uint16_t abspath_len;

  zcatw(inpath, sizeof(inpath), path, path_len);
  inpath[path_len] = 0;
  if((realpath(inpath, abspath)) == 0) {
    dprintf(2, "realpath(%s) : %s\n", inpath, strerror(errno));
    exit(1);
  }
  abspath_len = strlen(abspath);

  args_set_module_abspath(abspath, abspath_len);
  g_args.no_context_module = false;
}

void args_set_module_abspath(const char * restrict path, uint16_t path_len)
{
  uint64_t u64;

  u64 = hash64(0, path, path_len);
  g_args.context_module_id = u64;
}

void args_set_no_module()
{
  g_args.context_module_id = 0;
  g_args.no_context_module = true;
}

void args_command_start(struct command * restrict cmd, uint32_t flags)
{
  g_cmd = cmd;
  if(g_cmd->init) {
    g_cmd->init(g_cmd, flags);
  }
}

void args_command_finish()
{
  if(!g_cmd) {
    args_command_start(&build_command, 0);
  }
}
