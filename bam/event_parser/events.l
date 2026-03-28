/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%{
  #include "yyutil/scanner.h"
  #include "events.tab.h"
  #include "events.tokens.h"

  #define YYSTYPE union yyu_lval
  #define YYLTYPE yyu_location
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="events_yy"
%option stack

%x multilinecomment
%x quoted

%%

 /* single-line comments */
<INITIAL>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*           { LOCWRITE; }

 /* multiline comments are nestable */
<INITIAL,multilinecomment>[/][*]                        { LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]                                { LOCWRITE; POPSTATE; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<INITIAL>[\n]                                           { LOCRESET; }
<INITIAL>[\r\t ]+                                       { LOCWRITE; }

 /* tokens */
<INITIAL>[-![\]{}()+:=<>&|/%*^~]                        { return LEX(yytext[0]); }

 /* bools */
<INITIAL>(?i:true|false)                                { return LEXB(); }

 /* integers */
<INITIAL>[0-9]+                                         { return LEXI("%"SCNuMAX); }
<INITIAL>[-][0-9]+                                      { return LEXI("%"SCNdMAX); }
<INITIAL>(?i:0x[a-f0-9]+)                               { return LEXI("%"SCNxMAX); }

 /* floats */
<INITIAL>[+-]?[0-9]+[.][0-9]+                           { return LEXF(); }

 /* probably an id */
<INITIAL>("bm1"[qpzry9x8gf2tvdw0s3jn54khce6mua7l]{7,})  { return LEX(BECH32); }

 /* strings */
<INITIAL>[-_a-zA-Z0-9./+=%|]+                           { return LEXA(); }

 /* quoted strings */
<INITIAL>["]                                            { PUSHSTATE(quoted); return LEX(yytext[0]); }
<quoted>["]                                             { POPSTATE; return LEX(yytext[0]); }
<quoted>\\[\\"abtnvfrs0]                                { return LEXC(1); }
<quoted>\\x[0-9a-f]{2}                                  { return LEXH(2); }
<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { return LEX(STR); }

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
