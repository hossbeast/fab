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

  #include "request_parser.internal.h"
  #include "request.tab.h"
  #include "request.tokens.h"
  #include "selector_parser.h"
  #include "selector.h"
  #include "macros.h"

  #define YYSTYPE REQUEST_YYSTYPE
  #define YYLTYPE yyu_location

#define PARSE_SELECTOR do {                                             \
  request_parser * parser = containerof(yyextra, request_parser, yyu);  \
  size_t off = yytext - parser->yyu.yybuffer;                           \
  char * usr = parser->yyu.usrbuf + off;                                \
  size_t bufsz = parser->yyu.usrbufsize - off;                          \
  yyu_location subloc;                                                  \
    int r;                                                                \
  r = selector_parser_parse_partial(                                   \
      parser->selector_parser                                           \
    , usr                                                               \
    , bufsz                                                             \
    , "selector"                                                        \
    , &parser->yyu.loc                                                  \
    , &subloc                                                           \
    , &yylval->selector                                                 \
  );                                                                    \
    if(r != 0) {                                                          \
      return r;                                                           \
    }                                                                     \
  yyless(subloc.l);                                                     \
  LEXLOC(request_SELECTOR, &subloc);                                    \
} while(0);
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="request_yy"
%option stack

%x multilinecomment
%x _select _selector

/* bytes that cannot appear in the input */
ILL   [\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* single-line comments */
<_select,_selector>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*     |
<INITIAL>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*               { LOCWRITE; }

 /* multiline comments are nestable */
<_select,_selector>[/][*]                               |
<INITIAL,multilinecomment>[/][*]                        { LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]                                { LOCWRITE; POPSTATE; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<_select,_selector>[\n]                                 |
<INITIAL>[\n]                                           { LOCRESET; }
<_select,_selector>[\r\t ]+                             |
<INITIAL>[\r\t ]+                                       { LOCWRITE; }

 /* tokens */
<INITIAL>[[\]{}:=]                                      { LEX(yytext[0]); }

 /* invoke selector sub-parser */
<INITIAL>"select"                                       { PUSHSTATE(_select); LEX(SELECT); }
<INITIAL>"target-direct"                                { PUSHSTATE(_select); LEX(TARGET_DIRECT); }
<INITIAL>"target-transitive"                            { PUSHSTATE(_select); LEX(TARGET_TRANSITIVE); }
<_select>":"                                            { PUSHSTATE(_selector); LEX(':'); }
 /* consume whitespace in this parser */
<_selector>[^\n\r\t ][\x00-\xff]+                       { PARSE_SELECTOR; POPSTATE; POPSTATE; }

 /* enumerated tokenrange */
<INITIAL>[a-zA-Z0-9_][-a-zA-Z0-9_]*                     { LEXE(); }

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
