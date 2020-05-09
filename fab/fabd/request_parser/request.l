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

  #include "REQUEST.errtab.h"
  #include "request_parser.internal.h"
  #include "request.tab.h"
  #include "request.tokens.h"
  #include "selector_parser.h"
  #include "config_parser.h"
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
  fatal(selector_parser_parse_partial                                   \
    , parser->selector_parser                                           \
    , usr                                                               \
    , bufsz                                                             \
    , "selector"                                                        \
    , &parser->yyu.loc                                                  \
    , &subloc                                                           \
    , &yylval->selector                                                 \
  );                                                                    \
  yyless(subloc.l);                                                     \
  LEXLOC(request_SELECTOR, &subloc);                                    \
} while(0);

#define PARSE_CONFIG do {                                               \
  request_parser * parser = containerof(yyextra, request_parser, yyu);  \
  size_t off = yytext - parser->yyu.yybuffer;                           \
  char * usr = parser->yyu.usrbuf + off;                                \
  size_t bufsz = parser->yyu.usrbufsize - off;                          \
  yyu_location subloc;                                                  \
  fatal(config_parser_parse_partial                                     \
    , parser->config_parser                                             \
    , usr                                                               \
    , bufsz                                                             \
    , "config"                                                          \
    , &parser->yyu.loc                                                  \
    , &subloc                                                           \
    , &yylval->config                                                   \
  );                                                                    \
  yyless(subloc.l);                                                     \
  LEXLOC(request_CONFIG, &subloc);                                      \
} while(0);
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="request_yy"
%option stack

%x multilinecomment
%x _select _selector
%x _stage_config _config

/* bytes that cannot appear in the input */
ILL   [\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* single-line comments */
<_select,_selector>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*     |
<_stage_config,_config>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]* |
<INITIAL>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*               { LOCWRITE; }

 /* multiline comments are nestable */
<_select,_selector>[/][*]                               |
<_stage_config,_config>[/][*]                           |
<INITIAL,multilinecomment>[/][*]                        { lenter; LOCWRITE; PUSHSTATE(multilinecomment); finally : lcoda; }
<multilinecomment>[*][/]                                { lenter; LOCWRITE; POPSTATE; finally : lcoda; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<_select,_selector>[\n]                                 |
<_stage_config,_config>[\n]                             |
<INITIAL>[\n]                                           { LOCRESET; }
<_select,_selector>[\r\t ]+                             |
<_stage_config,_config>[\r\t ]+                         |
<INITIAL>[\r\t ]+                                       { LOCWRITE; }

 /* tokens */
<INITIAL>[[\]{}:=]                                      { lenter; LEX(yytext[0]); finally : lcoda; }

 /* invoke selector sub-parser */
<INITIAL>"select"                                       { lenter; PUSHSTATE(_select); LEX(SELECT); finally : lcoda; }
<INITIAL>"target-direct"                                { lenter; PUSHSTATE(_select); LEX(TARGET_DIRECT); finally : lcoda; }
<INITIAL>"target-transitive"                            { lenter; PUSHSTATE(_select); LEX(TARGET_TRANSITIVE); finally : lcoda; }
<_select>":"                                            { lenter; PUSHSTATE(_selector); LEX(':'); finally : lcoda; }
 /* consume whitespace in this parser */
<_selector>[^\n\r\t ][\x00-\xff]+                       { lenter; PARSE_SELECTOR; POPSTATE; POPSTATE; finally : lcoda; }

 /* invoke config sub-parser */
<INITIAL>"stage-config"                                 { lenter; PUSHSTATE(_stage_config); LEX(STAGE_CONFIG); finally : lcoda; }
<_stage_config>":"                                      { lenter; PUSHSTATE(_config); LEX(':'); finally : lcoda; }
 /* consume whitespace in this parser */
<_config>[^\n\r\t ][\x00-\xff]+                         { lenter; PARSE_CONFIG; POPSTATE; POPSTATE; finally : lcoda; }

 /* enumerated tokenrange */
<INITIAL>[a-zA-Z0-9_][-a-zA-Z0-9_]*                     { lenter; LEXE(); finally : lcoda; }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          lenter;
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          failf(REQUEST_ILLBYTE, "byte", "0x%02hhx", yytext[0]);
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          failf(REQUEST_ILLBYTE, "character", "'%1$c'(0x%1$02hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          POPSTATES;
                                                          _yyu_V = 0; /* return end-of-input */
                                                          finally : lcoda;
                                                        }
