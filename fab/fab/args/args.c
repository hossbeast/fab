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

#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "xlinux/mempolicy.h"

#include "narrator.h"
#include "narrator/units.h"

#include "fab/request.h"

#include "logger.h"
#include "logger/filter.h"
#include "logger/category.h"
#include "logger/arguments.h"

#include "args.h"
#include "logging.h"
#include "params.h"
#include "errtab/MAIN.errtab.h"

#include "macros.h"
#include "assure.h"
#include "memblk.h"

#define restrict __restrict

struct g_args * g_args;

//
//
//

static xapi usage(int valid, int version, int help, int logcats)
{
  enter;

  printf(
"fab : build optimally\n"
);
if(version)
{
  printf(" fab-" XQUOTE(FABVERSIONS)
#if DEVEL
  "+DEVEL"
#elif DEBUG
  "+DEBUG"
#endif
    " @ " XQUOTE(BUILDSTAMP)
    "\n"
  );
}

if(help)
{
  printf(
"\n"
"usage : fab [ [ <modifier> ] [ <selector> ] [ <option> ] [ <logexpr> ] ] ...\n"
"\n"
" --help     this message\n"
" --version  version information\n"
" --logs     logexpr listing\n"
"\n"
"----------- [ modifier ] -----------------------------------------------------------------\n"
"\n"
" +t|-t               (default) apply selectors to fabricate-list\n"
" +x|-x                         apply selectors to fabricate-exact-list\n"
" +n|-n                         apply selectors to fabricate-nofile-list\n"
" +d|-d                         apply selectors to discovery-list\n"
" +b|-b                         apply selectors to invalidate-list\n"
" +i|-i                         apply selectors to inspect-list\n"
" +q|-q                         apply selectors to query-list\n"
"\n"
"----------- [ options ] ------------------------------------------------------------------\n"
"\n"
" -p                            create buildplan only, implies +BPDUMP\n"
" -B                            kill fabd instance, if any. consider +b [ $! ]\n"
" -j <number>                   concurrency limit (0=unbounded, -1=based on detected CPUs)\n"
" -k <path>                     create buildscript at <path>\n"
" -K <varname>                  varname is settable at buildscript execution time\n"
#if DEVEL
" --license-standard    (default) buildscripts have the standard license\n"
" --license-fab                   buildscripts have the fab distribution license\n"
#endif
#if DEBUG || DEVEL
" --backtrace-pithy   (default) produce a summary of the callstack on error\n"
" --backtrace-full              produce a complete description of the callstack on error\n"
#endif
  );
}

if(logcats)
{
printf(
"\n"
"----------------- [ logs ] -----------------------------------------------------------------------\n"
"\n"
);

  
  fatal(logger_filter_pushs, 0, "+LOGGER");
  fatal(logger_categories_report);
  fatal(logger_filter_pop, 0);
}

printf(
"\n"
"For more information visit http://fabutil.org\n"
"\n"
);

  exit(!valid);

  finally : coda;
}

/// targets
//
// SUMMARY
//  
//
static xapi targets_process(char * restrict s)
{
  enter;

  // selectors apply to the following list(s)
  static uint8_t lists = TARGET_FABRICATE;
  static char mode = FABCORE_TARGET_ADD;  // +
  static size_t targetsa;

  if(s)
  {
    if(*s == '-' || *s == '+')
    {
      if(*s == '-')
        mode = FABCORE_TARGET_REMOVE;
      else if(*s == '+')
        mode = FABCORE_TARGET_ADD;

      lists = 0;

      while(*s)
      {
        if(*s == 't')
          lists |= TARGET_FABRICATE;
        if(*s == 'x')
          lists |= TARGET_FABRICATE_EXACT;
        if(*s == 'n')
          lists |= TARGET_FABRICATE_NOFILE;
        if(*s == 'b')
          lists |= TARGET_INVALIDATE;
      }
    }
    else
    {
      fatal(assure, &g_args->targets, sizeof(*g_args->targets), g_args->targetsl + 1, &targetsa);
      g_args->targets[g_args->targetsl] = (typeof(*g_args->targets)){
          mode  : mode
        , lists : lists
      };
      fatal(ixstrdup, &g_args->targets[g_args->targetsl++].text, s);
    }
  }
  
  finally : coda;
}

static xapi target_say(struct target * t, narrator * const restrict N)
{
  enter;

  says("{ ");
  if(t->mode == FABCORE_TARGET_ADD)
    sayc('+');
  else if(t->mode == FABCORE_TARGET_REMOVE)
    sayc('-');

  uint8_t x = 1;
  int first = 1;
  while(x)
  {
    if(t->lists & x)
    {
      if(!first)
        says("|");
      says(TARGET_STR(x));
      first = 0;
    }
    x <<= 1;
  }

  sayf(" %s", t->text);
  says(" }");

  finally : coda;
}

//
// [[ public ]]
//

void args_teardown()
{
  wfree(g_args);
}

xapi args_parse()
{
  enter;

  int help = 0;
  int version = 0;
  int logcats = 0;

  size_t buildscript_runtime_varsa = 0;

  fatal(xmalloc, &g_args, sizeof(*g_args));

  struct option longopts[] = {
/* informational */
          { "help"                        , no_argument , &help, 1 }
        , { "args"                        , no_argument , &help, 1 }
        , { "params"                      , no_argument , &help, 1 }
        , { "options"                     , no_argument , &help, 1 }
        , { "opts"                        , no_argument , &help, 1 }
        , { "version"                     , no_argument , &version, 1 }
        , { "vrs"                         , no_argument , &version, 1 }
        , { "vers"                        , no_argument , &version, 1 }
        , { "log"                         , no_argument , &logcats, 1 }
        , { "logs"                        , no_argument , &logcats, 1 }
        , { "logcat"                      , no_argument , &logcats, 1 }
        , { "logcats"                     , no_argument , &logcats, 1 }
        , { "logexpr"                     , no_argument , &logcats, 1 }
        , { "logexprs"                    , no_argument , &logcats, 1 }
        , { "o"                           , required_argument , 0, 0 }
        , { "op"                          , required_argument , 0, 0 }
        , { "o1"                          , no_argument , 0, 0 }
        , { "o2"                          , no_argument , 0, 0 }
        , { "o3"                          , no_argument , 0, 0 }
        , { "o4"                          , no_argument , 0, 0 }
        , { "o5"                          , no_argument , 0, 0 }
        , { "o6"                          , no_argument , 0, 0 }
        , { "o7"                          , no_argument , 0, 0 }
        , { "o8"                          , no_argument , 0, 0 }
        , { "o9"                          , no_argument , 0, 0 }
        , { "oa"                          , no_argument , 0, 0 }
        , { "oA"                          , no_argument , 0, 0 }
        , { "ob"                          , no_argument , 0, 0 }
        , { "oB"                          , no_argument , 0, 0 }
        , { "oc"                          , no_argument , 0, 0 }
        , { "oC"                          , no_argument , 0, 0 }
        , { "od"                          , no_argument , 0, 0 }
        , { "oD"                          , no_argument , 0, 0 }
        , { "oe"                          , no_argument , 0, 0 }
        , { "oE"                          , no_argument , 0, 0 }
        , { "of"                          , no_argument , 0, 0 }
        , { "oF"                          , no_argument , 0, 0 }

#if DEBUG || DEVEL
        , { "backtrace-pithy"             , no_argument , &g_args->mode_backtrace , MODE_BACKTRACE_PITHY }
        , { "backtrace-full"              , no_argument , &g_args->mode_backtrace , MODE_BACKTRACE_FULL }
        , { "backtrace"                   , no_argument , &g_args->mode_backtrace , MODE_BACKTRACE_FULL }
#endif
#if DEVEL
        , { "license-standard"              , no_argument , &g_args->mode_license   , MODE_LICENSE_STD }
        , { "license-fab"                   , no_argument , &g_args->mode_license   , MODE_LICENSE_FAB }
#endif

/* program switches */
// a
/* b - selection(s) apply to invalidate-list */
// c
/* d - selection(s) apply to inspect-list */
// e
/* f */ , { 0 , required_argument , 0     , 'f' }   // init-fabfile-path
// g
/* h */
// i - selection(s) apply to inspect-list */
/* j */ , { 0 , required_argument , 0     , 'j' }   // concurrency limit
/* k */ , { 0 , required_argument , 0     , 'k' }   // buildscript output path
// l 
// m
// n - selection(s) apply to fabricate-nofile-list */
// o
/* p */ , { 0 , no_argument       , 0     , 'p' }   // implies BPDUMP
/* q - selection(s) apply to query-list */
// r
// s
/* t - selection(s) apply to fabricate-list */
// u
/* v */ , { 0 , required_argument , 0     , 'v' }   // root-level variable definition
// w
/* x - selection(s) apply to fabricate-exact-list */
// y
// z
// A
/* B */ , { 0 , no_argument       , 0     , 'B' }   // global graph node invalidation
/* C - following selectors resolve against cwd */
// D
// E
/* F - following selectors resolve against init-fabfile-dir */
// G
// H
/* I */ , { 0 , required_argument , 0     , 'I' }   // directory to search for invocations
// J
/* K */ , { 0 , required_argument , 0     , 'K' }   // bs-runtime variables
// L
// M
// N
// O
// P
// Q
// R
// S
// T
// U
// V
// W
// X
// Y
// Z
        , { }
  };

  char* switches =
    // getopt options
    "-"

    // no-argument switches
    "cpB"

    // with-argument switches
    "f:j:k:v:I:K:"
  ;

  // disable getopt error messages
  opterr = 0;

  int x;
  int longindex;
  while((x = getopt_long(g_argc, g_argv, switches, longopts, &longindex)) != -1)
  {
    if(x == 0)
    {
    }
    else if(x == 'j')
    {
      int n = 0;
      if(sscanf(optarg, "%d%n", &g_args->concurrency, &n) != 1 || n < -1)
      {
        failf(MAIN_BADARGS, "badly formed option for -j : '%s'", optarg);
      }
    }
    else if(x == 'k')
    {
      g_args->mode_build = MODE_BUILD_BUILDSCRIPT;
      iwfree(&g_args->buildscript_path);
      fatal(ixstrdup, &g_args->buildscript_path, optarg);
    }
    else if(x == 'p')
    {
      g_args->mode_build = MODE_BUILD_GENERATE;
    }
    else if(x == 'B')
    {
      g_args->invalidate_all = 1;
    }
    else if(x == 'K')
    {
      fatal(assure, &g_args->buildscript_runtime_vars, sizeof(*g_args->buildscript_runtime_vars), g_args->buildscript_runtime_varsl + 1, &buildscript_runtime_varsa);
      fatal(ixstrdup, &g_args->buildscript_runtime_vars[g_args->buildscript_runtime_varsl++], optarg);
    }
    else if(x == '?')
    {
      if(optopt)
      {
        failf(MAIN_BADARGS, "unknown switch", "-%c", optopt);
      }
      else
      {
        fails(MAIN_BADARGS, "unknown argument", g_argv[optind-1]);
      }
    }
    else
    {
      fatal(targets_process, optarg);
    }
  }

  // options following --
  for(; optind < g_argc; optind++)
    fatal(targets_process, g_argv[optind]);

  if(help || version || logcats)
  {
    fatal(usage, 1, 1, help, logcats);
  }

  // CPU count heuristic
  if(g_args->concurrency == -1)
  {
    if(g_params.procs)
    {
      g_args->procs = (float)g_params.procs * 1.2f;
    }
    else
    {
      g_args->procs = 8;
    }
  }

  finally : coda;
}

xapi args_report()
{
  enter;

  char space[512];
  int token = 0;

  // log cmdline args under ARGS
  if(g_args->mode_build)
    logf(L_ARGS         , "(%5s) mode-build             =%s", "k/p", MODE_STR(g_args->mode_build));

  if(g_args->buildscript_path)
    logf(L_ARGS         , "(  %c  ) buildscript-path       =%s", 'k', g_args->buildscript_path);

  int x;
  for(x = 0; x < g_args->buildscript_runtime_varsl; x++)
    logf(L_ARGS     , "(  %c  ) bs-runtime-var(s)      =%s", 'K', g_args->buildscript_runtime_vars[x]);

#if DEBUG || DEVEL
  if(g_args->mode_backtrace)
    logf(L_ARGS         , "(  %c  ) mode-backtrace         =%s", ' ', MODE_STR(g_args->mode_backtrace));
#endif
#if DEVEL
  if(g_args->mode_license)
    logf(L_ARGS         , "(  %c  ) mode-license             =%s", ' ', MODE_STR(g_args->mode_license));
#endif

  if(g_args->concurrency == -1)
    snprintf(space, sizeof(space)  , "heuristic(%d)", g_args->procs);
  else
    snprintf(space, sizeof(space)  , "%d", g_args->concurrency);

  if(g_args->concurrency)
    logf(L_ARGS        , "(  %c  ) concurrency            =%s", 'j', space);

  if(g_args->invalidate_all)
    logf(L_ARGS         , "(  %c  ) invalidate-all         =%s", 'B', g_args->invalidate_all ? "yes" : "no");

  for(x = 0; x < g_args->targetsl; x++)
  {
    fatal(log_start, L_ARGS, &token);
    logf(0, "(  %c  ) targets(s)            =", ' ');
    fatal(target_say, &g_args->targets[x], log_narrator(&token));
    fatal(log_finish, &token);
  }

finally:
  fatal(log_finish, &token);
coda;
}

xapi args_request_collate(memblk * const restrict mb, fab_request ** const restrict request)
{
  enter;

  size_t x;
  int mpc = 0;

  fatal(mempolicy_push, memblk_getpolicy(mb), &mpc);
  fatal(fab_request_create, request, g_params.pid);

  // overwrite logging filters
  fatal(fab_request_commandf, *request, FABCORE_CONFIGURATION_MERGE, "logging.console.filters [ \"%s\" ]", g_logvsl ? g_logvs : "+ERROR,INFO");

  if(g_args->mode_build)
    fatal(fab_request_commandu8, *request, FABCORE_MODE_BUILD, g_args->mode_build);

#if DEVEL
  if(g_args->mode_license == MODE_LICENSE_FAB)
    fatal(fab_request_commandf, *request, FABCORE_CONFIGURATION_MERGE, "license_mode \"fab\"", g_args->mode_license);
  else // MODE_LICENSE_STD
    fatal(fab_request_commandf, *request, FABCORE_CONFIGURATION_MERGE, "license_mode \"standard\"");
#endif

  if(g_args->buildscript_path || g_args->buildscript_runtime_varsl)
  {
    if(!g_args->buildscript_path)
      failf(MAIN_BADARGS, "error", "-K is specified along with -k");

    fatal(fab_request_commands, *request, FABCORE_BUILDSCRIPT_PATH, g_args->buildscript_path);
    fatal(fab_request_command, *request, FABCORE_BUILDSCRIPT_VARS | FABCORE_CLEAR);

    for(x = 0; x < g_args->buildscript_runtime_varsl; x++)
      fatal(fab_request_commands, *request, FABCORE_BUILDSCRIPT_VARS | FABCORE_PUSH, g_args->buildscript_runtime_vars[x]);
  }

  if(g_args->targetsl)
  {
    fatal(fab_request_command, *request, FABCORE_CLEAR | FABCORE_TARGET_FABRICATE);
    fatal(fab_request_command, *request, FABCORE_CLEAR | FABCORE_TARGET_FABRICATE_EXACT);
    fatal(fab_request_command, *request, FABCORE_CLEAR | FABCORE_TARGET_FABRICATE_NOFILE);
    fatal(fab_request_command, *request, FABCORE_CLEAR | FABCORE_TARGET_INVALIDATE);

    for(x = 0; x < g_args->targetsl; x++)
    {
      if(g_args->targets[x].lists & TARGET_FABRICATE)
        fatal(fab_request_command_target, *request, FABCORE_TARGET_FABRICATE | FABCORE_PUSH, g_args->targets[x].mode, g_args->targets[x].text);

      if(g_args->targets[x].lists & TARGET_FABRICATE_EXACT)
        fatal(fab_request_command_target, *request, FABCORE_TARGET_FABRICATE_EXACT | FABCORE_PUSH, g_args->targets[x].mode, g_args->targets[x].text);

      if(g_args->targets[x].lists & TARGET_FABRICATE_NOFILE)
        fatal(fab_request_command_target, *request, FABCORE_TARGET_FABRICATE_NOFILE | FABCORE_PUSH, g_args->targets[x].mode, g_args->targets[x].text);

      if(g_args->targets[x].lists & TARGET_INVALIDATE)
        fatal(fab_request_command_target, *request, FABCORE_TARGET_INVALIDATE | FABCORE_PUSH, g_args->targets[x].mode, g_args->targets[x].text);
    }
  }

  fatal(fab_request_command, *request, FABCORE_CONFIGURATION_APPLY);

  if(g_args->invalidate_all)
    fatal(fab_request_command, *request, FABCORE_INVALIDATE_ALL);

finally:
  mempolicy_unwind(&mpc);
coda;
}
