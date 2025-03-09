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

  #define PARSE_LOOKUP_PATTERN ({                                            \
    configset_parser * parser = containerof(yyextra, configset_parser, yyu); \
    size_t off = yytext - parser->yyu.yybuffer;                              \
    char * usr = parser->yyu.usrbuf + off;                                   \
    size_t bufsz = parser->yyu.usrbufsize - off;                             \
    yyu_location subloc;                                                     \
    int r;                                                                   \
    r = lookup_pattern_parse_partial(                                        \
        parser->pattern_parser                                               \
      , usr                                                                  \
      , bufsz                                                                \
      , parser->yyu.fname                                                    \
      , &parser->yyu.loc                                                     \
      , &subloc                                                              \
      , &yylval->lookup_pattern                                              \
    );                                                                       \
    if(r == 0) {                                                             \
      yyless(subloc.l);                                                      \
      r = LEXLOC(configset_LOOKUP_PATTERN, &subloc);                         \
    }                                                                        \
    r;                                                                       \
  })

  #define PARSE_INCLUDE_PATTERN ({                                           \
    configset_parser *parser = containerof(yyextra, configset_parser, yyu);  \
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
      , &yylval->include_pattern                                             \
    );                                                                       \
    if(r == 0) {                                                             \
      yyless(subloc.l);                                                      \
      r = LEXLOC(configset_INCLUDE_PATTERN, &subloc);                        \
    }                                                                        \
    r;                                                                       \
  })

  #define PARSE_MATCH_PATTERN ({                                             \
    configset_parser *parser = containerof(yyextra, configset_parser, yyu);  \
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
      , &yylval->match_pattern                                               \
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
%x fstrees0
%x fstrees1
%x fstrees2
%x fstrees3
%x fstrees4
%x include_pattern0
%x match_pattern0
%x lookup_pattern0
%x matches
%x pattern0
%x pattern1

%%

 /* single-line comments */
<*>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*   { LOCWRITE; }

 /* multiline comments are nestable */
<INITIAL,matches,fstrees4,multilinecomment>[/][*]       { LOCWRITE; PUSHSTATE(multilinecomment); }
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
<fstrees4>[-![\]()+:=<>&|/%*^~]                         { return LEX(yytext[0]); }

 /* bools */
<INITIAL,fstrees4>(?i:true|false)                       { return LEXB(); }

 /* sub-parsers */
<INITIAL>"fstrees"                                      { PUSHSTATE(fstrees0); return LEX(FSTREES); }
<fstrees0>[:]                                           { POPSTATE; PUSHSTATE(fstrees1); return LEX(yytext[0]); }
<fstrees1>[{]                                           { POPSTATE; PUSHSTATE(fstrees2); return LEX(yytext[0]); }
<fstrees2>[}]                                           { POPSTATE; return LEX(yytext[0]); }
<fstrees2>[\x00-\xff]                                   { yyless(0); PUSHSTATE(fstrees3); PUSHSTATE(include_pattern0); }
<fstrees3>[:]                                           { return LEX(yytext[0]); }
<fstrees3>[{]                                           { POPSTATE; PUSHSTATE(fstrees4); return LEX(yytext[0]); }
<fstrees3>[}]                                           { POPSTATE; return LEX(yytext[0]); }
<fstrees4>[{]                                           { PUSHSTATE(fstrees4); return LEX(yytext[0]); }
<fstrees4>[}]                                           { POPSTATE; return LEX(yytext[0]); }
<include_pattern0>[^\n\r\t ][\x00-\xff]+                { POPSTATE; return PARSE_INCLUDE_PATTERN; }
<lookup_pattern0>[^\n\r\t ][\x00-\xff]+                 { POPSTATE; return PARSE_LOOKUP_PATTERN; }

<INITIAL>"pattern"                                      { PUSHSTATE(pattern0); return LEX(PATTERN); }
<pattern0>[:]                                           { POPSTATE; PUSHSTATE(pattern1); return LEX(yytext[0]); }
<pattern0>[^:]                                          { POPSTATE; yyless(0); }
<pattern1>[\x00-\xff]                                   { POPSTATE; yyless(0); PUSHSTATE(lookup_pattern0); }

<INITIAL>"special"                                      { PUSHSTATE(matches); return LEX(SPECIAL); }
<INITIAL>"exclude"                                      { PUSHSTATE(matches); return LEX(EXCLUDE); }
<matches>[{:=]                                          { return LEX(yytext[0]); }
<matches>"}"                                            { POPSTATE; return LEX(yytext[0]); }
<matches>[^}:=]                                         { yyless(0); PUSHSTATE(match_pattern0); }
<match_pattern0>[^\n\r\t ][\x00-\xff]+                  { int r = PARSE_MATCH_PATTERN; POPSTATE; return r; }

 /* integers */
<INITIAL,fstrees4>[0-9]+                                { return LEXI("%"SCNuMAX); }
<INITIAL,fstrees4>[-][0-9]+                             { return LEXI("%"SCNdMAX); }
<INITIAL,fstrees4>(?i:0x[a-f0-9]+)                      { return LEXI("%"SCNxMAX); }

 /* floats */
<INITIAL,fstrees4>[+-]?[0-9]+[.][0-9]+                  { return LEXF(); }

 /* strings and enumerated tokens */
<INITIAL,fstrees4>[-a-zA-Z0-9_./+=@]+                   { return LEXA(); }

 /* quoted strings */
<INITIAL>["]                                            { PUSHSTATE(quoted); return LEX(yytext[0]); }
<quoted>["]                                             { POPSTATE; return LEX(yytext[0]); }
 /* sysvars */
<INITIAL>"$bm_homedir"                                  { return LEX(BM_HOMEDIR); }
<INITIAL>"$bm_ipcdir"                                   { return LEX(BM_IPCDIR); }
<INITIAL>"$bm_basedir"                                  { return LEX(BM_BASEDIR); }
<INITIAL>"$bm_bamd_id"                                  { return LEX(BM_BAMD_ID); }
<INITIAL>"$bm_bamd_pid"                                 { return LEX(BM_BAMD_PID); }

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
