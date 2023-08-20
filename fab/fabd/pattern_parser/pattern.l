/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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
<*>[\n]                                                 { LOCRESET; yyless(0); return LEX(yytext[0]); }
<*>[\r\t ]                                              { LOCWRITE; yyless(0); return LEX(yytext[0]); }

 /* tokens */
<INITIAL>[*][*]                                         { return LEX(STARSTAR); }
<INITIAL>[/][/]                                         { int r = LEX(SLASH2); yyless(1); return r; }
<INITIAL>[+(){}<>:*?,/~^D]                              { return LEX(yytext[0]); }
<INITIAL>[.][/]                                         { int r = LEX(yytext[0]); yyless(1); return r; }

 /* integers */
<INITIAL>[0-9]+                                         { return LEXI("%"SCNuMAX); }

 /* named tokens and unquoted strings */
<INITIAL>[a-zA-Z0-9_][-a-zA-Z0-9_]+                     { return LEXA(); }
<INITIAL>[-a-zA-Z_.]+                                   { return LEX(STR); }

 /* scanmodes, quoted strings */
<INITIAL>[[]                                            { PUSHSTATE(bracketed); return LEX(yytext[0]); }
<bracketed>[\]]                                         { POPSTATE; return LEX(yytext[0]); }

<INITIAL>["]                                            { PUSHSTATE(quoted); return LEX(yytext[0]); }
<quoted>["]                                             { POPSTATE; return LEX(yytext[0]); }

 /* var refs */
<INITIAL>[$]                                            { PUSHSTATE(varref); return LEX(yytext[0]); }
<varref>[{]                                             { PUSHSTATE(varref2); return LEX(yytext[0]); }
<varref>[?^D]                                           { POPSTATE; return LEX(yytext[0]); }
<varref>[a-zA-Z]+                                       { POPSTATE; return LEX(STR); }
<varref>[0-9]+                                          { POPSTATE; return LEXI("%"SCNuMAX); }
<varref2>[-+?]                                          { return LEX(yytext[0]); }
<varref2>[a-zA-Z]+                                      { return LEX(STR); }
<varref2>[}]                                            { POPSTATE; POPSTATE; return LEX(yytext[0]); }

 /* crefs to non ascii bytes */
<quoted>\\[0abtnvfrs\\"]                                { return LEXC(1); }
<bracketed>\\[0abtnvfrs]                                { return LEXC(1); }

 /* 1-char hexrefs to non-ascii bytes */
<bracketed,quoted>(?i:\\x\{[0-9a-f]\})                  { return LEXH(3); }

 /* 2-char hexrefs to non-ascii bytes */
<bracketed,quoted>(?i:\\x[01][0-9a-f])                  { return LEXH(2); }
<bracketed,quoted>(?i:\\x\{[01][0-9a-f]\})              { return LEXH(3); }
<bracketed,quoted>(?i:\\x[7][f])                        { return LEXH(2); }
<bracketed,quoted>(?i:\\x\{[7][f]\})                    { return LEXH(3); }
<bracketed,quoted>(?i:\\x[89a-f][0-9a-f])               { return LEXH(2); }
<bracketed,quoted>(?i:\\x\{[89a-f][0-9a-f]\})           { return LEXH(3); }

 /* 2-char ascii hexrefs coerced to wref */
<bracketed,quoted>(?i:\\x[0-9a-f]{2})                   { return LEXH(2); }
<bracketed,quoted>(?i:\\x\{[0-9a-f]{2}\})               { return LEXH(3); }

<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { return LEX(STR); }
<bracketed>[-^]                                         { return LEX(yytext[0]); }
<bracketed>[^-\\\x00-\x1F\x7F-\xFF]                     { return LEX(CHAR); }

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
