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
<*>[\n]                                                 { LOCRESET; yyless(0); LEX('\n'); }
<*>[\r\t ]                                              { LOCWRITE; yyless(0); LEX(yytext[0]); }

 /* tokens */
<INITIAL>[*][*]                                         { LEX(STARSTAR); }
<INITIAL>[/][/]                                         { LEX(SLASH2); yyless(1); }
<INITIAL>[+(){}<>:*?,/~^D]                              { LEX(yytext[0]); }
<INITIAL>[.][/]                                         { LEX(yytext[0]); yyless(1); }

 /* integers */
<INITIAL>[0-9]+                                         { LEXI("%"SCNuMAX); }

 /* named tokens and unquoted strings */
<INITIAL>[a-zA-Z0-9_][-a-zA-Z0-9_]+                     { LEXA(); }
<INITIAL>[-a-zA-Z_.]+                                   { LEX(STR); }

 /* scanmodes, quoted strings */
<INITIAL>[[]                                            { PUSHSTATE(bracketed); LEX(yytext[0]); }
<bracketed>[\]]                                         { POPSTATE; LEX(yytext[0]); }

<INITIAL>["]                                            { PUSHSTATE(quoted); LEX(yytext[0]); }
<quoted>["]                                             { POPSTATE; LEX(yytext[0]); }

 /* var refs */
<INITIAL>[$]                                            { PUSHSTATE(varref); LEX(yytext[0]); }
<varref>[{]                                             { PUSHSTATE(varref2); LEX(yytext[0]); }
<varref>[?^D]                                           { POPSTATE; LEX(yytext[0]); }
<varref>[a-zA-Z]+                                       { POPSTATE; LEX(STR); }
<varref>[0-9]+                                          { POPSTATE; LEXI("%"SCNuMAX); }
<varref2>[-+?]                                          { LEX(yytext[0]); }
<varref2>[a-zA-Z]+                                      { LEX(STR); }
<varref2>[}]                                            { POPSTATE; POPSTATE; LEX(yytext[0]); }

 /* crefs to non ascii bytes */
<quoted>\\[0abtnvfrs\\"]                                { LEXC(1); }
<bracketed>\\[0abtnvfrs]                                { LEXC(1); }

 /* 1-char hexrefs to non-ascii bytes */
<bracketed,quoted>(?i:\\x\{[0-9a-f]\})                  { LEXH(3); }

 /* 2-char hexrefs to non-ascii bytes */
<bracketed,quoted>(?i:\\x[01][0-9a-f])                  { LEXH(2); }
<bracketed,quoted>(?i:\\x\{[01][0-9a-f]\})              { LEXH(3); }
<bracketed,quoted>(?i:\\x[7][f])                        { LEXH(2); }
<bracketed,quoted>(?i:\\x\{[7][f]\})                    { LEXH(3); }
<bracketed,quoted>(?i:\\x[89a-f][0-9a-f])               { LEXH(2); }
<bracketed,quoted>(?i:\\x\{[89a-f][0-9a-f]\})           { LEXH(3); }

 /* 2-char ascii hexrefs coerced to wref */
<bracketed,quoted>(?i:\\x[0-9a-f]{2})                   { LEXH(2); }
<bracketed,quoted>(?i:\\x\{[0-9a-f]{2}\})               { LEXH(3); }

<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { LEX(STR); }
<bracketed>[-^]                                         { LEX(yytext[0]); }
<bracketed>[^-\\\x00-\x1F\x7F-\xFF]                     { LEX(CHAR); }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          yyu_errorf(YYUXTRA, "invalid byte 0x%02hhx", yytext[0]);
                                                          return 0;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          yyu_errorf(YYUXTRA, "character '%1$c'(0x%1$02hhx)", yytext[0]);
                                                          return 0;
                                                        }
<<EOF>>                                                 {
                                                          POPSTATES;
                                                          return 0;
                                                        }
