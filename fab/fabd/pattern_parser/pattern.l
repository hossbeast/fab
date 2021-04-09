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

%{
  #include "yyutil/scanner.h"

  #include "PATTERN.errtab.h"
  #include "pattern_parser.internal.h"
  #include "match_pattern.tab.h"
  #include "match_pattern.tokens.h"

#include <unistd.h>
#include "types.h"

  #define YYSTYPE struct pattern_yystype
  #define YYLTYPE yyu_location
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="pattern_yy"
%option stack

%x bracketed
%x quoted
%x varref varref2

%%

 /* whitespace not recognized */
<*>[\n]                                                 { lenter; LOCRESET; yyless(0); LEX('\n'); finally : lcoda; }
<*>[\r\t ]                                              { lenter; LOCWRITE; yyless(0); LEX(yytext[0]); finally : lcoda; }

 /* tokens */
<INITIAL>[*][*]                                         { lenter; LEX(STARSTAR); finally : lcoda; }
<INITIAL>[/][/]                                         { lenter; LEX(SLASH2); yyless(1); finally : lcoda; }
<INITIAL>[+(){}<>:*?,/~^D]                              { lenter; LEX(yytext[0]); finally : lcoda; }
<INITIAL>[.][/]                                         { lenter; LEX(yytext[0]); yyless(1); finally : lcoda; }

 /* integers */
<INITIAL>[0-9]+                                         { lenter; LEXI("%"SCNuMAX); finally : lcoda; }

 /* named tokens and unquoted strings */
<INITIAL>[a-zA-Z0-9_][-a-zA-Z0-9_]+                     { lenter; LEXA(); finally : lcoda; }
<INITIAL>[-a-zA-Z_.]+                                   { lenter; LEX(STR); finally : lcoda; }

 /* scanmodes, quoted strings */
<INITIAL>[[]                                            { lenter; PUSHSTATE(bracketed); LEX(yytext[0]); finally : lcoda; }
<bracketed>[\]]                                         { lenter; POPSTATE; LEX(yytext[0]); finally : lcoda; }

<INITIAL>["]                                            { lenter; PUSHSTATE(quoted); LEX(yytext[0]); finally : lcoda; }
<quoted>["]                                             { lenter; POPSTATE; LEX(yytext[0]); finally : lcoda; }

 /* var refs */
<INITIAL>[$]                                            { lenter; PUSHSTATE(varref); LEX(yytext[0]); finally : lcoda; }
<varref>[{]                                             { lenter; PUSHSTATE(varref2); LEX(yytext[0]); finally : lcoda; }
<varref>[?^D]                                           { lenter; POPSTATE; LEX(yytext[0]); finally : lcoda; }
<varref>[a-zA-Z]+                                       { lenter; POPSTATE; LEX(STR); finally : lcoda; }
<varref>[0-9]+                                          { lenter; POPSTATE; LEXI("%"SCNuMAX); finally : lcoda; }
<varref2>[-+?]                                          { lenter; LEX(yytext[0]); finally : lcoda; }
<varref2>[a-zA-Z]+                                      { lenter; LEX(STR); finally : lcoda; }
<varref2>[}]                                            { lenter; POPSTATE; POPSTATE; LEX(yytext[0]); finally : lcoda; }

 /* crefs to non ascii bytes */
<quoted>\\[0abtnvfrs\\"]                                { lenter; LEXC(1); finally : lcoda; }
<bracketed>\\[0abtnvfrs]                                { lenter; LEXC(1); finally : lcoda; }

 /* 1-char hexrefs to non-ascii bytes */
<bracketed,quoted>(?i:\\x\{[0-9a-f]\})                  { lenter; LEXH(3); finally : lcoda; }

 /* 2-char hexrefs to non-ascii bytes */
<bracketed,quoted>(?i:\\x[01][0-9a-f])                  { lenter; LEXH(2); finally : lcoda; }
<bracketed,quoted>(?i:\\x\{[01][0-9a-f]\})              { lenter; LEXH(3); finally : lcoda; }
<bracketed,quoted>(?i:\\x[7][f])                        { lenter; LEXH(2); finally : lcoda; }
<bracketed,quoted>(?i:\\x\{[7][f]\})                    { lenter; LEXH(3); finally : lcoda; }
<bracketed,quoted>(?i:\\x[89a-f][0-9a-f])               { lenter; LEXH(2); finally : lcoda; }
<bracketed,quoted>(?i:\\x\{[89a-f][0-9a-f]\})           { lenter; LEXH(3); finally : lcoda; }

 /* 2-char ascii hexrefs coerced to wref */
<bracketed,quoted>(?i:\\x[0-9a-f]{2})                   { lenter; LEXH(2); finally : lcoda; }
<bracketed,quoted>(?i:\\x\{[0-9a-f]{2}\})               { lenter; LEXH(3); finally : lcoda; }

<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { lenter; LEX(STR); finally : lcoda; }
<bracketed>[-^]                                         { lenter; LEX(yytext[0]); finally : lcoda; }
<bracketed>[^-\\\x00-\x1F\x7F-\xFF]                     { lenter; LEX(CHAR); finally : lcoda; }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          lenter;
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          failf(PATTERN_ILLBYTE, "byte", "0x%02hhx", yytext[0]);
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          failf(PATTERN_ILLBYTE, "character", "'%1$c'(0x%1$02hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          POPSTATES;
                                                          _yyu_V = 0;       /* return end-of-input */
                                                          finally : lcoda;
                                                        }
