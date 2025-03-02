/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%{
  #include "yyutil/scanner.h"

  #include "configset_parser/glue.h"
  #include "configset_parser.h"
  #include "configset.tab.h"
  #include "configset.tokens.h"

  #include "pattern_parser.h"

  #define YYSTYPE CONFIGSET_YYSTYPE
  #define YYLTYPE yyu_location

  #define PARSE_INCLUDE_PATTERN ({                                           \
    configset_parser * parser = containerof(yyextra, configset_parser, yyu); \
    size_t off = yytext - parser->yyu.yybuffer;                              \
    char * usr = parser->yyu.usrbuf + off;                                   \
    size_t bufsz = parser->yyu.usrbufsize - off;                             \
    yyu_location subloc;                                                     \
    int r;                                                                   \
    r = include_pattern_parse_partial(                                       \
        parser->pattern_parser                                               \
      , usr                                                                  \
      , bufsz                                                                \
      , parser->yyu.fname                                                    \
      , &parser->yyu.loc                                                     \
      , &subloc                                                              \
      , &yylval->pattern                                                     \
    );                                                                       \
    if(r == 0) {                                                             \
      yyless(subloc.l);                                                      \
      r = LEXLOC(configset_INCLUDE_PATTERN, &subloc);                        \
    }                                                                        \
    r;                                                                       \
  })

  #define PARSE_MATCH_PATTERN ({                                             \
    configset_parser * parser = containerof(yyextra, configset_parser, yyu); \
    size_t off = yytext - parser->yyu.yybuffer;                              \
    char * usr = parser->yyu.usrbuf + off;                                   \
    size_t bufsz = parser->yyu.usrbufsize - off;                             \
    yyu_location subloc;                                                     \
    int r;                                                                   \
    r = match_pattern_parse_partial(                                         \
        parser->pattern_parser                                               \
      , usr                                                                  \
      , bufsz                                                                \
      , parser->yyu.fname                                                    \
      , &parser->yyu.loc                                                     \
      , &subloc                                                              \
      , &yylval->pattern                                                     \
    );                                                                       \
    if(r == 0) {                                                             \
      yyless(subloc.l);                                                      \
      r = LEXLOC(configset_MATCH_PATTERN, &subloc);                          \
    }                                                                        \
    r;                                                                       \
  })
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="configset_yy"
%option stack

%x multilinecomment
%x quoted
%x fstrees1
%x fstrees2
%x fstree1
%x fstree2
%x fstree3
%x include_pattern
%x exclude
%x match_pattern
%x anchor

/* bytes that cannot appear in the input */
ILL   [\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* single-line comments */
<*>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*   { LOCWRITE; }

 /* multiline comments are nestable */
<INITIAL,exclude,multilinecomment>[/][*]                { LOCWRITE; PUSHSTATE(multilinecomment); }
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
<INITIAL,fstree3>(?i:true|false)                        { return LEXB(); }

 /* sub-parsers */
<INITIAL>"fstrees"                                      { PUSHSTATE(fstrees1); return LEX(FSTREES); }
<fstrees1>[:]                                           { PUSHSTATE(fstrees2); return LEX(yytext[0]); }
<fstrees2>[{]                                           { PUSHSTATE(fstree1); return LEX(yytext[0]); }
<fstrees2>[}]                                           { POPSTATE; POPSTATE; return LEX(yytext[0]); }

<fstree1>[}]                                            { yyless(0); POPSTATE; }
<fstree1>[\x00-\xff]                                    { yyless(0); PUSHSTATE(fstree2); PUSHSTATE(include_pattern); }
<fstree2>[:]                                            { PUSHSTATE(fstree3); return LEX(yytext[0]); }
<fstree2>[\x00-\xff]                                    { yyless(0); POPSTATE; }
<fstree3>[:{]                                           { return LEX(yytext[0]); }
<fstree3>[}]                                            { POPSTATE; POPSTATE; return LEX(yytext[0]); }
<include_pattern>[^\n\r\t ][\x00-\xff]+                 { POPSTATE; int r = PARSE_INCLUDE_PATTERN; return r; }

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

 /* strings and enumerated tokens */
<INITIAL,fstree3>[-a-zA-Z0-9_./+=]+                     { return LEXA(); }

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
                                                          yyu_errorf(yyuparser, yylloc, "unexpected character '%1$c'(0x%1$02hhx)", yytext[0]);
                                                          return 0;
                                                        }
<<EOF>>                                                 {
                                                          return 0;
                                                        }
