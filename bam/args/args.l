/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%{
  #include "yyutil/scanner.h"
  #include "args.tab.h"
  #include "args.tokens.h"

  #define YYSTYPE union yyu_lval
  #define YYLTYPE yyu_location
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="args_yy"
%option stack

%x adhoc
%x multilinecomment
%x cmd
%x main
%x module
%x namespace
%x plugin
%x blame

%%
 /* jump into the cmd state (recognizes command names) and prepare to
  * jump into the main state (does not recognize them) */
<INITIAL>[\x20-\x7E]+                                   { PUSHSTATE(main); PUSHSTATE(cmd); yyless(0); }

 /* single-line comments */
<*>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*                 { LOCWRITE; }

 /* multiline comments are nestable */
<*>[/][*]                                               { LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]                                { LOCWRITE; POPSTATE; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<*>[\n]                                                 { LOCRESET; }
<*>[\r\t ]                                              { LOCWRITE; }

 /* integers */
<*>[-]?[0-9]+                                           { return LEXI("%"SCNuMAX); }
 /* probably an id */
<*>("bm1"[qpzry9x8gf2tvdw0s3jn54khce6mua7l]{7,})        { return LEX(BECH32); }

 /* tokens */
<*>[-+bdghuvxt]                                         { return LEX(yytext[0]); }

 /*
  * list of command names ; after their first appearance, these will be parsed as STR tokens
  *
  * a simpler way to do this would be to allow these strings to always parse as tokens, e.g.
  * with LEXA, and then roll them up into the word nonterminal, and let the grammar sort it out
  * positionally.
  *
  * that would almost work, but it needs to be organized this way instead because parsing
  * build-arguments is the default state, that is, the build command is the default (without the
  * BUILD token appearing) and of course one of the build arguments is simply the name or path to
  * a target file.
  *
  * consider targeting a file that shares a name with one of these tokens. in the setup where these
  * strings parse as tokens, the user could still target such a file by using a path prefix, e.g.
  * ./configsets. Doing this adds complexity here, but removes that burden from the user.
  *
  * in short
  *  1/ this list of tokens matches the reduction of command in args.y,
  *  2/ each command with subcommands has an exclusive start state
  *  3/ the list of tokens in the next state, is this list, plus all subcommand names
  */

 /* command names stop being distinct tokens after the first appearance */
<main>"args"                  |
<main>"autobuild"             |
<main>"auxout"                |
<main>"build"                 |
<main>"build_record"          |
<main>"build_records"         |
<main>"config"                |
<main>"configset"             |
<main>"configsets"            |
<main>"cwd"                   |
<main>"describe"              |
<main>"envs"                  |
<main>"events"                |
<main>"file"                  |
<main>"formula"               |
<main>"formulas"              |
<main>"fsname"                |
<main>"fsnames"               |
<main>"fstree"                |
<main>"fstrees"               |
<main>"goals"                 |
<main>"info"                  |
<main>"kill"                  |
<main>"ls"                    |
<main>"module"                |
<main>"modules"               |
<main>"namespace"             |
<main>"namespaces"            |
<main>"plan"                  |
<main>"plugin"                |
<main>"plugins"               |
<main>"reconcile"             |
<main>"rma"                   |
<main>"rmas"                  |
<main>"rule"                  |
<main>"rules"                 |
<main>"ruleset"               |
<main>"rulesets"              |
<main>"blame"                 |
<main>"stats"                 |
<main>"stderr"                |
<main>"stdin"                 |
<main>"stdout"                |
<main>"touch"                 |
<main>"tree"                  |
<main>"vars"                  |
<main>"varset"                |
<main>"varsets"               { POPSTATE; return LEX(STR); }

 /* adhoc command */
<cmd>"adhoc"                  { POPSTATE; PUSHSTATE(adhoc); return LEX(ADHOC); }
<adhoc>[\x20-\x7E]+           { return LEX(STR); }

 /* namespace subcommands */
<cmd>"namespace"              { POPSTATE; PUSHSTATE(namespace); return LEXA(); }
<namespace>"fsname"           |
<namespace>"fsnames"          { POPSTATE; return LEXA(); }
 /* global argument options */
<namespace>[-][-_a-zA-Z0-9]+  { return LEXA(); }
 /* enums and unquoted strings */
<namespace>[-+_a-zA-Z0-9./%]+ { return LEX(STR); }

 /* module subcommands */
<cmd>"module"                 { POPSTATE; PUSHSTATE(module); return LEXA(); }
<module>"envs"                |
<module>"vars"                |
<module>"rules"               |
<module>"rulesets"            |
<module>"rma"                 |
<module>"rmas"                { POPSTATE; return LEXA(); }
<module>[-][-_a-zA-Z0-9]+     { return LEXA(); }
<module>[-+_a-zA-Z0-9./%]+    { return LEX(STR); }

 /* plugin subcommands */
<cmd>"plugin"                 { POPSTATE; PUSHSTATE(plugin); return LEXA(); }
<plugin>"stdin"               |
<plugin>"stdout"              |
<plugin>"stderr"              |
<plugin>"auxout"              { POPSTATE; return LEXA(); }
<plugin>[-][-_a-zA-Z0-9]+     { return LEXA(); }
<plugin>[-+_a-zA-Z0-9./%]+    { return LEX(STR); }

 /* blame subcommands */
<cmd>"blame"                  { POPSTATE; PUSHSTATE(blame); return LEXA(); }
<blame>"stdin"                |
<blame>"stdout"               |
<blame>"stderr"               |
<blame>"auxout"               |
<blame>"args"                 |
<blame>"envs"                 |
<blame>"file"                 |
<blame>"cwd"                  { POPSTATE; return LEXA(); }
<blame>[-][-_a-zA-Z0-9]+      { return LEXA(); }
<blame>[-+_a-zA-Z0-9./%]+     { return LEX(STR); }

 /* enums and unquoted strings */
<main>[-+_a-zA-Z0-9./%]+      { return LEXA(); }

 /* name, but not a command name ; leave */
<cmd>[a-z][-_a-z0-9]+         { POPSTATE; return LEXA(); }
 /* not a command name ; stay */
<cmd>[-+_a-zA-Z0-9./%]+       { return LEXA(); }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          LOCWRITE;
                                                          yyu_errorf(yyuparser, yylloc, "invalid byte 0x%02hhx", yytext[0]);
                                                          return 0;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          LOCWRITE;
                                                          yyu_errorf(yyuparser, yylloc, "character '%1$c'(0x%1$02hhx)", yytext[0]);
                                                          return 0;
                                                        }
<<EOF>>                                                 {
                                                          return 0;
                                                        }
