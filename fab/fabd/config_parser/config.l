/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%{
  #include "yyutil/scanner.h"

  #include "config_parser/glue.h"
  #include "config_parser.h"
  #include "config.tab.h"
  #include "config.tokens.h"

  #include "pattern_parser.h"

  #define YYSTYPE CONFIG_YYSTYPE
  #define YYLTYPE yyu_location

  #define PARSE_INCLUDE_PATTERN ({                                        \
    config_parser * parser = containerof(yyextra, config_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    int r;                                                                \
    r = include_pattern_parse_partial(                                    \
        parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(config_INCLUDE_PATTERN, &subloc);                        \
    }                                                                     \
    r;                                                                    \
  })

  #define PARSE_MATCH_PATTERN ({                                          \
    config_parser * parser = containerof(yyextra, config_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    int r;                                                                \
    r = match_pattern_parse_partial(                                      \
        parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(config_MATCH_PATTERN, &subloc);                          \
    }                                                                     \
    r;                                                                    \
  })
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="config_yy"
%option stack

%x multilinecomment
%x quoted
%x include
%x include_pattern
%x exclude
%x match_pattern
%x anchor

/* bytes that cannot appear in the input */
ILL   [\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* single-line comments */
<INITIAL,include,exclude>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*           { LOCWRITE; }

 /* multiline comments are nestable */
<INITIAL,include,exclude,multilinecomment>[/][*]        { LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]                                { LOCWRITE; POPSTATE; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<*>[\n]                                                 { LOCRESET; }
<*>[\r\t ]+                                             { LOCWRITE; }

 /* tokens */
<INITIAL>[-![\]{}()+:=<>&|/%*^~]                        { return LEX(yytext[0]); }

 /* bools */
<INITIAL>(?i:true|false)                                { return LEXB(); }

 /* sub-parsers */
<INITIAL>"include"                                      { PUSHSTATE(include); return LEX(INCLUDE); }
<include>[{:=]                                          { return LEX(yytext[0]); }
<include>"}"                                            { POPSTATE; return LEX(yytext[0]); }
<include>[^}:=]                                         { yyless(0); PUSHSTATE(include_pattern); }
<include_pattern>[^\n\r\t ][\x00-\xff]+                 { int r = PARSE_INCLUDE_PATTERN; POPSTATE; return r; }

<INITIAL>"exclude"                                      { PUSHSTATE(exclude); return LEX(EXCLUDE); }
<exclude>[{:=]                                          { return LEX(yytext[0]); }
<exclude>"}"                                            { POPSTATE; return LEX(yytext[0]); }
<exclude>[^}:=]                                         { yyless(0); PUSHSTATE(match_pattern); }
<match_pattern>[^\n\r\t ][\x00-\xff]+                   { int r = PARSE_MATCH_PATTERN; POPSTATE; return r; }

 /* integers */
<INITIAL>[0-9]+                                         { return LEXI("%"SCNuMAX); }
<INITIAL>[-][0-9]+                                      { return LEXI("%"SCNdMAX); }
<INITIAL>(?i:0x[a-f0-9]+)                               { return LEXI("%"SCNxMAX); }

 /* floats */
<INITIAL>[+-]?[0-9]+[.][0-9]+                           { return LEXF(); }

 /* strings */
<INITIAL>[-a-zA-Z0-9_./+=]+                             { return LEXA(); }

 /* quoted strings */
<INITIAL>["]                                            { PUSHSTATE(quoted); return LEX(yytext[0]); }
<quoted>["]                                             { POPSTATE; return LEX(yytext[0]); }
<quoted>\\[\\"abtnvfrs0]                                { return LEXC(1); }
<quoted>\\x[0-9a-f]{2}                                  { return LEXH(2); }
<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { return LEX(STR); }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          yyu_errorf(yyuparser, yylloc, "invalid byte 0x%02hhx", yytext[0]);
                                                          return 0;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          yyu_errorf(yyuparser, yylloc, "unexpected character '%1$c'(0x%1$02hhx)", yytext[0]);
                                                          return 0;
                                                        }
<<EOF>>                                                 {
                                                          POPSTATES;
                                                          return 0;
                                                        }
