/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include "valyria/pstring.h"
#include "common/snarf.h"
#include "narrator.h"
#include "bacon/load.h"
#include "bacon/merge.h"
#include "bacon/parser.h"
#include "bacon/eval.h"
#include "bacon/expr.h"
#include "xlinux/xstdlib.h"

#include "args.h"

__thread int32_t tid;
__thread int *errnop;

int main(int argc, char ** argv, char ** envp)
{
  bacon_parser * parser = 0;
  char * text = 0;
  size_t text_len = 0;
  size_t text_alloc;
  const char * fname;
  char * s;
  size_t sl;
  bacon_value * vars = 0;
  bacon_value * val = 0;
  bacon_expr * expr = 0;
  struct stat stb;

  tid = syscall(SYS_gettid);
  errnop = &errno;

#ifdef CONFIG_DEVEL
  backtracef_setup();
#endif

  bacon_load();

  // parse cmdline arguments
  args_parse(argc, argv);

  bacon_parser_alloc(&parser);

  /* consume stdin if pipe */
  if(fstat(0, &stb) != 0) {
    dprintf(2, "%s\n", strerror(errno));
    exit(1);
  }
  switch(stb.st_mode & S_IFMT)
  {
    case S_IFIFO:
    case S_IFREG:
    case S_IFSOCK:
      fsnarf(&text, &text_len, 0);
      if(bacon_parser_parse(parser, text, text_len, "-stdin-", BACON_TYPE_SET, &vars) != 0) {
        dprintf(2, "b : %.*s\n", parser->yyu->error_len, parser->yyu->error_str);
        exit(1);
      }
  }
  free(text);
  text = 0;

  if(!g_args.arg) {
    goto end;
  }
  if(g_args.input == IN_FILE)
  {
    snarfs(&text, &text_len, g_args.arg);
    s = text;
    sl = text_len;
    fname = g_args.arg;
  }
  else
  {
    sl = strlen(g_args.arg);
    text_alloc = 0;
    psinitx(&text, &text_len, &text_alloc, sl + 2);
    psloadw(&text, &text_len, &text_alloc, g_args.arg, sl);
    s = text;
    fname = "-argv-";
  }

  if(g_args.oper == OP_MERGE)
  {
    if(bacon_parser_parse(parser, s, sl, fname, g_args.container, &val) != 0) {
      dprintf(2, "bacon parse error : %.*s\n", parser->yyu->error_len, parser->yyu->error_str);
      exit(1);
    }
    if(vars == 0) {
      vars = val;
    } else {
      bacon_merge(parser, &vars, val);
      bacon_parser_value_free(parser, val);
    }
    val = 0;
  }
  else if(g_args.oper == OP_EVAL)
  {
    if(bacon_parser_expr_parse(parser, s, sl, fname, &expr) != 0) {
      dprintf(2, "expr parse error : %.*s\n", parser->yyu->error_len, parser->yyu->error_str);
      exit(1);
    }

    bacon_eval(parser, expr, vars, &val);
    bacon_parser_value_free(parser, vars);
    vars = val;
    val = 0;
  }
  free(text);
  text = 0;

end:
  if(vars) {
    bacon_value_say(vars, g_narrator_stdout);
    narrator_says(g_narrator_stdout, "\n");
  }

  bacon_parser_value_free(parser, val);
  bacon_parser_value_free(parser, vars);
  bacon_parser_expr_free(parser, expr);
  bacon_parser_free(parser);

  args_teardown();
  bacon_unload();
}
