/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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

  #define PARSE_SELECTOR ({                                               \
    request_parser * parser = containerof(yyextra, request_parser, yyu);  \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    int r;                                                                \
    r = selector_parser_parse_partial(                                    \
        parser->selector_parser                                           \
      , usr                                                               \
      , bufsz                                                             \
      , "selector"                                                        \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->selector                                                 \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(request_SELECTOR, &subloc);                              \
    }                                                                     \
    r;                                                                    \
  })
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="request_yy"
%option stack

%x multilinecomment
%x quoted
%x _select _selector

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
<INITIAL>[[\]{}:=]                                      { return LEX(yytext[0]); }
<INITIAL>(?i:true|false)                                { return LEXB(); }

 /* integers */
<INITIAL>[-][0-9]+                                      { return LEXI("%"SCNdMAX); }
<INITIAL>[0-9]+                                         { return LEXI("%"SCNuMAX); }
<INITIAL>(?i:0x[0-9a-f]+)                               { return LEXI("%"SCNxMAX); }
<INITIAL>[0-9a-f]{8}                                    { return LEXI("%"SCNxMAX); }
<INITIAL>[0-9a-f]{16}                                   { return LEXI("%"SCNxMAX); }

 /* invoke selector sub-parser */
<INITIAL>"select"                                       { PUSHSTATE(_select); return LEX(SELECT); }
<INITIAL>"target-direct"                                { PUSHSTATE(_select); return LEX(TARGET_DIRECT); }
<INITIAL>"target-transitive"                            { PUSHSTATE(_select); return LEX(TARGET_TRANSITIVE); }
<_select>":"                                            { PUSHSTATE(_selector); return LEX(':'); }
 /* consume whitespace in this parser */
<_selector>[^\n\r\t ][\x00-\xff]+                       { int r = PARSE_SELECTOR; POPSTATE; POPSTATE; return r; }

 /* enums and unquoted strings */
<INITIAL>[a-zA-Z0-9_/.+][-a-zA-Z0-9_/.+]*               { return LEXA(); }
<INITIAL>[-a-zA-Z0-9_/.}]+                              { return LEX(STR); }

 /* quoted strings -- no escapes */
<INITIAL>["]                                            { PUSHSTATE(quoted); return LEX(yytext[0]); }
<quoted>["]                                             { POPSTATE; return LEX(yytext[0]); }
<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { return LEX(STR); }


<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          LOCWRITE;
                                                          yyu_errorf(yyuparser, yylloc, "invalid byte 0x%02hhx", yytext[0]);
                                                          return 0;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          LOCWRITE;
                                                          yyu_errorf(yyuparser, yylloc, "unexpected character '%1$c'(0x%1$02hhx)", yytext[0]);
                                                          return 0;
                                                        }
<<EOF>>                                                 {
                                                          return 0;
                                                        }
