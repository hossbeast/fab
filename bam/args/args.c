/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/assure.h"
#include "common/snarf.h"
#include "yyutil/parser.h"
#include "lorien/path_normalize.h"

#include "params.h"

#include "macros.h"

/* flex and bison do not agree on these names */
#define YYSTYPE yyu_lval
#define YYLTYPE yyu_location

#include "args.lex.h"
#include "args.states.h"
#include "args.tab.h"
#include "args.tokens.h"

#if CONFIG_DEVEL
#define CLIENT_SYSTEM_CONFIG_PATH    "/etc/bam/client_config""%""devel"
#define CLIENT_USER_CONFIG_PATH      "$HOME/.bam/client_config""%""devel"
#define DAEMON_SYSTEM_CONFIG_PATH    "/etc/config.bam""%""devel"
#define DAEMON_USER_CONFIG_PATH      "$HOME/.bam/config.bam""%""devel"
#else
#define CLIENT_SYSTEM_CONFIG_PATH    "/etc/bam/client_config"
#define CLIENT_USER_CONFIG_PATH      "$HOME/.bam/client_config"
#define DAEMON_SYSTEM_CONFIG_PATH    "/etc/config.bam"
#define DAEMON_USER_CONFIG_PATH      "$HOME/.bam/config.bam"
#endif

#include "git-state.h"

static YYU_VTABLE(args_vtable, args, args);
typeof(g_args) g_args;

static char *g_argvs;
static size_t g_argvsa;
static size_t g_argvsl;

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

  if(cmd && cmd->usage) {
    cmd->usage(cmd);
  } else {
    printf(
"usage: bam [command | option]..\n"
"\n"
"commands\n"
" build              (default) carry out the build, mark files fresh\n"
" autobuild                    as build, but remain connected\n"
" config                       show the effective configuration\n"
" describe                     show nodes metadata\n"
" events                       subscribe to and print events\n"
" goals                        show goals\n"
" info                         show metadata\n"
" ls                           show directory listings\n"
" tree                         show directory listings as a tree\n"
" plan                         show the stage sequence for the last build\n"
" reconcile                    reload everything\n"
" stats                        show global, or per-node counters\n"
" touch                        invalidate nodes\n"
"\n"
"inspect\n"
" build-record(s)              list or show build records\n"
" blame <path>                 show the most recent record of updating a file\n"
" rma <id> run                 run an rma and see its matches\n"
"\n"
"objects\n"
" configset(s)                 list or show configsets\n"
" formula(s)                   list or show formulas\n"
" fstree(s)                    list or show fstrees\n"
" module(s)                    list or show modules\n"
" namespace(s)                 list or show namespaces\n"
" plugin(s)                    list or show plugins\n"
" rma(s)                       list or show rmas\n"
" rule(s)                      list or show rules\n"
" ruleset(s)                   list or show modules\n"
" varset(s)                    list or show varsets\n"
"\n"
"other\n"
" adhoc                        issue an adhoc request\n"
" kill                         kill bamd if its running\n"
    );
  }
  printf(
"\n"
"global options\n"
" --help | -h                  print this message\n"
" --version | -V               print version information\n"
" -v                           print more stuff\n"
" -K                           first kill bamd if its running\n"
" -m path/to/module            set the context module\n"
" --no-module                  unset the context module\n"
" --no-color                   disable terminal colorization\n"
#if CONFIG_DEVEL
" --no-launch                  require bamd to already be running\n"
#endif
"\n"
"bam client config files\n"
" %s\n"
" %s\n"
"\n"
"bamd daemon config files\n"
" %s\n"
" %s\n"
"\n"
"client environment variables\n"
" BAM_IPC_DIR           the ipc dir\n"
" XDG_RUNTIME_DIR       else, $XDG_RUNTIME_DIR/bam is the ipc dir\n"
"\n"
"bam %s - build optimally\n"
" https://git.sr.ht/~hossbeast/bam\n"
    , CLIENT_SYSTEM_CONFIG_PATH
    , CLIENT_USER_CONFIG_PATH
    , DAEMON_SYSTEM_CONFIG_PATH
    , DAEMON_USER_CONFIG_PATH
    , git_describe
  );

  exit(0);
}

/* remove leading and trailing whitespace */
static void trim(char ** restrict s, uint16_t * restrict l)
{
  while(*l) {
    switch((*s)[0]) {
      case ' ':
      case '\n':
        (*s)++;
        (*l)--;
        continue;
    }
    switch((*s)[(*l) - 1]) {
      case ' ':
      case '\n':
        (*l)--;
        continue;
    }
    break;
  }
}

static int parse_args(yyu_parser * restrict parser, char * argvs, uint16_t argvsl, const char * restrict fname)
{
  int r;

  trim(&argvs, &argvsl);

  if(argvsl == 0) {
    return 0;
  }

  argvs[argvsl + 0] = 0;
  argvs[argvsl + 1] = 0;

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
  r = 0;
  if(text_len != 0) {
    if((r = yyu_parse(parser, text, text_len + 2, path, YYU_INPLACE, 0, 0))) {
      fprintf(stderr, "%.*s\n", parser->error_len, parser->error_str);
    }
  }

  free(text);
  return r;
}

void args_cleanup()
{
  free(g_argvs);
}

void args_parse(int argc, char ** argv)
{
  yyu_parser yyu;
  size_t len;
  int x;
  int r;

  g_args.context_module_id_len = zcatw(
      g_args.context_module_id
    , sizeof(g_args.context_module_id)
    , g_params.cwd_module_id, g_params.cwd_module_id_len
  );
  g_args.context_module_use = true;
  g_args.context_module_expect = false;

  memset(&yyu, 0, sizeof(yyu));
  yyu_parser_init(&yyu, &args_vtable);
  yyu_parser_init_tokens(&yyu, args_token_table, args_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(&yyu, args_numstates, args_statenumbers, args_statenames);

  /* parse well-known config files */
  if(parse_file(&yyu, CLIENT_SYSTEM_CONFIG_PATH) != 0) {
    exit(1);
  }
  if(parse_file(&yyu, CLIENT_USER_CONFIG_PATH) != 0) {
    exit(1);
  }

  /* join arguments-list into a single space-delimited string which has
   * room for two trailing null bytes */
  for(x = 1; x < argc; x++)
  {
    len = strlen(argv[x]);
    assure(&g_argvs, 1, g_argvsl + len + 3, &g_argvsa);
    memcpy(g_argvs + g_argvsl, argv[x], len);
    memcpy(g_argvs + g_argvsl + len, " ", 1);
    g_argvsl += len + 1;
  }

  r = parse_args(&yyu, g_argvs, g_argvsl, "-args-");
  yyu_parser_destroy(&yyu);

  if(r != 0) {
    exit(1);  // error printed earlier
  } else if(g_args.help) {
    usage(g_cmd);
  } else if(g_args.version) {
    version();
  }

  /* defaults */
  if(g_cmd->name == 0) {
    command_set(&build_command);
  }

  RUNTIME_ASSERT(g_cmd->name);
}

void args_set_module_path(const char * restrict module_path, uint16_t module_path_len)
{
  char path[PATH_MAX];
  uint16_t path_len;
  path_normalize_context ctx;

  memset(&ctx, 0, sizeof(ctx));
  ctx.base = g_params.cwd;
  ctx.base_len = g_params.cwd_len;
  path_len = path_normalize(&ctx, path, sizeof(path), module_path, module_path_len);

  g_args.context_module_id_len = module_compute_id(
      g_args.context_module_id
    , sizeof(g_args.context_module_id)
    , path
    , path_len
  );
  g_args.context_module_use = true;
  g_args.context_module_expect = true;
}

void args_set_no_module()
{
  g_args.context_module_id_len = 0;
  g_args.context_module_use = false;
  g_args.context_module_expect = false;
}
