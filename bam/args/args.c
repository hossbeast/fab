/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/assure.h"
#include "common/snarf.h"
#include "common/hash.h"
#include "xlinux/xstdlib.h"
#include "yyutil/parser.h"

#include "args.h"
#include "info.h"
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
    , git_describe
  );

  if(cmd && cmd->usage) {
    cmd->usage(cmd);
    printf("\n");
  }

  printf(
"commands\n"
" adhoc                 adhoc request\n"
" autobuild             as build, but remain connected\n"
" build       (default) carry out the build, mark files fresh\n"
" buildplan             show the stage sequence for the last build\n"
" config                show effective config\n"
" configset(s)          list configsets\n"
" describe              show node info\n"
" events                subscribe to and print events\n"
" formula(s)            list formulas\n"
" fstree(s)             list fstrees\n"
" goals                 show goals\n"
" info                  show metadata\n"
" kill                  kill bamd if its running\n"
" ls                    print directory node listings\n"
" module(s)             list modules\n"
" plugin(s)             list plugins\n"
" reconcile             reload everything\n"
" rma(s)                list rmas\n"
" rule(s)               list rules\n"
" ruleset(s)            list modules\n"
" stats                 show global, or per-node counters\n"
" touch                 invalidate nodes\n"
" tree                  print directory tree listings\n"
" varset(s)             list varsets\n"
"\n"
"global options\n"
" --help | -h           print this message\n"
" --version | -V        print version information\n"
" -v                    print more stuff\n"
" -B                    first, global invalidation\n"
" -K                    first, kill bamd if its running\n"
" -m path/to/module     set the context module\n"
" --no-module           unset the context module\n"
" --no-color            disable terminal colorization\n"
#if CONFIG_DEVEL
" --no-launch           require bamd to already be running\n"
#endif
  );

  printf(
"\n"
"client config files\n"
" "SYSTEM_CONFIG_PATH"\n"
" "USER_CONFIG_PATH"\n"
"\n"
"\n"
"daemon config files\n"
" /etc/config.bam\n"
" $HOME/.bam/config.bam\n"
"\n"
"client environment variables\n"
" BAM_IPC_DIR           the ipc dir\n"
" XDG_RUNTIME_DIR       else, $XDG_RUNTIME_DIR/bam is the ipc dir\n"
"\n"
"build optimally        https://git.sr.ht/~hossbeast/bam\n"
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

static int parse_file(yyu_parser * restrict parser, const char * restrict pathspec)
{
  char buf[PATH_MAX];
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
    home = getenv("HOME");
    z = 0;
    z += zcats(buf + z, sizeof(buf) - z, home);
    z += zcats(buf + z, sizeof(buf) - z, "/");
    z += zcats(buf + z, sizeof(buf) - z, pathspec + 2);
    buf[z] = 0;
    path = buf;
  }
  else if(len >= 6 && memcmp(pathspec, "$HOME/", 6) == 0)
  {
    home = getenv("HOME");
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
  command_set(&build_command);

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

  r = parse(&yyu, argvs, argvsl, "-args-");
  yyu_parser_destroy(&yyu);

  if(r != 0) {
    exit(1);  // error printed earlier
  } else if(g_args.help) {
    usage(g_cmd);
  } else if(g_args.version) {
    version();
  }

  RUNTIME_ASSERT(g_cmd);

  if(!g_args.no_context_module) {
    g_args.context_module_id_len = zcatw(g_args.context_module_id, sizeof(g_args.context_module_id), g_params.cwd_hash, g_params.cwd_hash_len);
  }
}

void args_set_module_path(const char * restrict path, uint16_t path_len)
{
  g_args.context_module_id_len = params_path_hash(g_args.context_module_id, sizeof(g_args.context_module_id), path, path_len);
  g_args.no_context_module = false;
}

void args_set_no_module()
{
  g_args.context_module_id_len = 0;
  g_args.no_context_module = true;
}
