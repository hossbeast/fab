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
  #include <stdio.h>
  #include <inttypes.h>

  // contains much of the implemention of the scanner
  struct transform_xtra;
  #define YYU_EXTRA_TYPE struct transform_xtra
  #include "yyutil/scanner.h"

  #include "transform/transform.def.h"
  #include "transform/transform.tab.h"
  #include "transform/transform.tokens.h"
  #include "genscan.internal.h"
  #include "operators.internal.h"

  #include "macros.h"

  /// LEX
  //
  // SUMMARY
  //  parse a whitespace or character token with no semantic value at an offset from yytext
  //
  // PARAMETERS
  //  token - token to parse
  //  ldel  - offset at which the location for this token begins
  //
  #define LEX(token, ldel)      \
    fatal(lexify                \
      , token                   \
      , yylval                  \
      , yylloc                  \
      , yyextra                 \
      , yytext                  \
      , yyleng                  \
      , ldel                    \
      , 0                       \
      , 0                       \
    )

  /// LEXV
  //
  // SUMMARY
  //  parse a token that has some associated semantic value
  //
  // PARAMETERS
  //  token - token to parse
  //  vdel  - offset at which to parse the semantic value
  //  v     - pointer to semantic value, if any
  //
  #define LEXV(token, vdel, v)  \
    fatal(lexify                \
      , token                   \
      , yylval                  \
      , yylloc                  \
      , yyextra                 \
      , yytext                  \
      , yyleng                  \
      , 0                       \
      , vdel                    \
      , v                       \
    )

  /// lexify
  //
  // SUMMARY
  //  populate semantic value, invoke yyu_lexify
  //
  static xapi lexify(const int token, YYSTYPE * const lval, yyu_location * const lloc, transform_xtra * const xtra, char * const text, const int leng, const int ldel, const int vdel, void * const v)
  {
    enter;

    // populate semantic value if any on a per token basis
    if(token == CREF)
    {
      if(text[vdel] == 'a')
        lval->ref = 0x07;
      else if(text[vdel] == 'b')
        lval->ref = 0x08;
      else if(text[vdel] == 't')
        lval->ref = 0x09;
      else if(text[vdel] == 'n')
        lval->ref = 0x0a;
      else if(text[vdel] == 'v')
        lval->ref = 0x0b;
      else if(text[vdel] == 'f')
        lval->ref = 0x0c;
      else if(text[vdel] == 'r')
        lval->ref = 0x0d;
      else if(text[vdel] == 's')
        lval->ref = 0x20;
    }
    else if(token == BREF)
      sscanf(text + vdel, "%d", &lval->ref);
    else if(token == HREF)
      sscanf(text + vdel, "%x", &lval->ref);
    else if(token == I64)
      sscanf(text + vdel, "%"SCNd64, &lval->i64);
    else if(token == OP)
      lval->op = *(operator **)v;

    fatal(yyu_lexify, token, lval, sizeof(*lval), lloc, xtra, text, leng, ldel, token == LF);

    finally : coda;
  }
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="transform_yy"
%option extra-type="transform_xtra*"
%option stack

%x ws
%x multilinecomment
%x emptyarg

%x slash_norefs slash_dorefs
%x comma_norefs comma_dorefs
%x dot_norefs dot_dorefs
%x colon_norefs colon_dorefs
%x semicolon_norefs semicolon_dorefs

%x braces braces_norefs braces_dorefs
%x brackets brackets_norefs brackets_dorefs
%x parens parens_norefs parens_dorefs
%x angles angles_norefs angles_dorefs

/* bytes that cannot appear in the string being scanned */
ILL   [\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* single-line comments */
<INITIAL,ws>[#][^\x00-\x08\x0A-\x1F\x7F-\xFF]*          { LOCWRITE; }

 /* multiline comments are nestable */
<INITIAL,ws,multilinecomment>[/][*]                     { lenter; LOCWRITE; PUSHSTATE(multilinecomment); finally : lcoda; }
<multilinecomment>[*][/]                                { lenter; LOCWRITE; POPSTATE; finally : lcoda; }
<multilinecomment>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+   { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /*
 ** In the INITIAL state, check whether a string is an operator name. If not, leave it to be scanned by
 ** the current scanmode. In either case, push ws so that we only get back to the INITIAL state when
 ** processing a scanmode directive
 **
 */

<INITIAL,ws>[\n]                                        { lenter; LEX(LF, 0); yield(LF); finally : lcoda; }
<INITIAL,ws>[\t ]+                                      { lenter; LEX(WS, 0); yield(WS); finally : lcoda; }

<INITIAL>[-a-zA-Z]+                                     {
                                                          lenter;

                                                          PUSHSTATE(ws);
                                                          typeof(*yylval->op) * op;
                                                          if((op = op_lookup(yytext, yyleng)))
                                                          {
                                                            LEXV(OP, 0, &op); yield(OP);
                                                          }
                                                          else
                                                          {
                                                            PUSHSTATE(genscan_startcondition_initial[yyextra->scanmode]);
                                                            yyless(0);
                                                          }

                                                          finally : lcoda;
                                                        }
<INITIAL>[^-\x00-\x08\x0B-\x1F\x7F-\xFFa-zA-Z\n\t ?]+   {
                                                          lenter;

                                                          PUSHSTATE(ws);
                                                          PUSHSTATE(genscan_startcondition_initial[yyextra->scanmode]);
                                                          yyless(0);

                                                          finally : lcoda;
                                                        }

 /* following ws, an alpha string must be an operator name or mnemonic */
<ws>[-a-zA-Z]+                                          {
                                                          lenter;

                                                          typeof(*yylval->op) * op;
                                                          if((op = op_lookup(yytext, yyleng)))
                                                          {
                                                            LEXV(OP, 0, &op); yield(OP);
                                                          }
                                                          else
                                                          {
                                                            failf(LISTWISE_NXOP, "operator", "%s", yytext);
                                                          }

                                                          finally : lcoda;
                                                        }
 /* after processing a scanmode directive, revert to the INITIAL scanner */
<INITIAL>[?]{1,2}[/,.:;{[(<]                            { LOCWRITE; yyextra->scanmode = genscan_parse(yytext, yyleng); }
<ws>[\t ]*[?]{1,2}[/,.:;{[(<][\t ]*                     { lenter; LOCWRITE; yyextra->scanmode = genscan_parse(yytext, yyleng); POPSTATE; finally : lcoda; }

 /* process anything else with the current scanmode */
<ws>[^\x00-\x08\x0B-\x1F\x7F-\xFF]                      { lenter; PUSHSTATE(genscan_startcondition_initial[yyextra->scanmode]); finally : lcoda; yyless(0); }

 /* fabricate an empty-string STR token */
<emptyarg>[\x00-\xff]                                   { lenter; POPSTATE; yyless(0); LEX(STR, 1); yield(STR); finally : lcoda; }

 /* slash-delimited scanner */
<slash_norefs,slash_dorefs>[/]                          { lenter; LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<slash_norefs,slash_dorefs>[/][/]                       { lenter; PUSHSTATE(emptyarg); yyless(1); LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<slash_norefs,slash_dorefs>[\n\t ]                      { lenter; POPSTATE; finally : lcoda; yyless(0); }
<slash_norefs,slash_dorefs>-?[0-9]+                     { lenter; LEXV(I64, 0, 0); yield(I64); finally : lcoda; }

<slash_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /]+     { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

<slash_dorefs>\\[\\\n\t /]                              { lenter; LEX(STR, 1); yield(STR); finally : lcoda; }
<slash_dorefs>\\[abtnvfrs]                              { lenter; LEXV(CREF, 1, 0); yield(CREF); finally : lcoda; }
<slash_dorefs>\\x[0-9a-fA-F]{1,2}                       { lenter; LEXV(HREF, 2, 0); yield(HREF); finally : lcoda; }
<slash_dorefs>\\x\{[0-9a-fA-F]{1,2}\}                   { lenter; LEXV(HREF, 3, 0); yield(HREF); finally : lcoda; }
<slash_dorefs>\\[0-9]+                                  { lenter; LEXV(BREF, 1, 0); yield(BREF); finally : lcoda; }
<slash_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /\\]+   { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

 /* comma-delimited scanner */
<comma_norefs,comma_dorefs>[,]                          { lenter; LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<comma_norefs,comma_dorefs>[,][,]                       { lenter; PUSHSTATE(emptyarg); yyless(1); LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<comma_norefs,comma_dorefs>[\n\t ]                      { lenter; POPSTATE; finally : lcoda; yyless(0); }
<comma_norefs,comma_dorefs>-?[0-9]+                     { lenter; LEXV(I64, 0, 0); yield(I64); finally : lcoda; }

<comma_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ,]+     { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

<comma_dorefs>\\[\\\n\t ,]                              { lenter; LEX(STR, 1); yield(STR); finally : lcoda; }
<comma_dorefs>\\[abtnvfrs]                              { lenter; LEXV(CREF, 1, 0); yield(CREF); finally : lcoda; }
<comma_dorefs>\\x[0-9a-fA-F]{1,2}                       { lenter; LEXV(HREF, 2, 0); yield(HREF); finally : lcoda; }
<comma_dorefs>\\x\{[0-9a-fA-F]{1,2}\}                   { lenter; LEXV(HREF, 3, 0); yield(HREF); finally : lcoda; }
<comma_dorefs>\\[0-9]+                                  { lenter; LEXV(BREF, 1, 0); yield(BREF); finally : lcoda; }
<comma_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ,\\]+   { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

 /* dot-delimited scanner */
<dot_norefs,dot_dorefs>[.]                              { lenter; LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<dot_norefs,dot_dorefs>[.][.]                           { lenter; PUSHSTATE(emptyarg); yyless(1); LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<dot_norefs,dot_dorefs>[\n\t ]                          { lenter; POPSTATE; finally : lcoda; yyless(0); }
<dot_norefs,dot_dorefs>-?[0-9]+                         { lenter; LEXV(I64, 0, 0); yield(I64); finally : lcoda; }

<dot_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t .]+       { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

<dot_dorefs>\\[\\\n\t .]                                { lenter; LEX(STR, 1); yield(STR); finally : lcoda; }
<dot_dorefs>\\[abtnvfrs]                                { lenter; LEXV(CREF, 1, 0); yield(CREF); finally : lcoda; }
<dot_dorefs>\\x[0-9a-fA-F]{1,2}                         { lenter; LEXV(HREF, 2, 0); yield(HREF); finally : lcoda; }
<dot_dorefs>\\x\{[0-9a-fA-F]{1,2}\}                     { lenter; LEXV(HREF, 3, 0); yield(HREF); finally : lcoda; }
<dot_dorefs>\\[0-9]+                                    { lenter; LEXV(BREF, 1, 0); yield(BREF); finally : lcoda; }
<dot_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t .\\]+     { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

 /* colon-delimited scanner */
<colon_norefs,colon_dorefs>[:]                          { lenter; LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<colon_norefs,colon_dorefs>[:][:]                       { lenter; PUSHSTATE(emptyarg); yyless(1); LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<colon_norefs,colon_dorefs>[\n\t ]                      { lenter; POPSTATE; finally : lcoda; yyless(0); }
<colon_norefs,colon_dorefs>-?[0-9]+                     { lenter; LEXV(I64, 0, 0); yield(I64); finally : lcoda; }

<colon_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t :]+     { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

<colon_dorefs>\\[\\\n\t :]                              { lenter; LEX(STR, 1); yield(STR); finally : lcoda; }
<colon_dorefs>\\[abtnvfrs]                              { lenter; LEXV(CREF, 1, 0); yield(CREF); finally : lcoda; }
<colon_dorefs>\\x[0-9a-fA-F]{1,2}                       { lenter; LEXV(HREF, 2, 0); yield(HREF); finally : lcoda; }
<colon_dorefs>\\x\{[0-9a-fA-F]{1,2}\}                   { lenter; LEXV(HREF, 3, 0); yield(HREF); finally : lcoda; }
<colon_dorefs>\\[0-9]+                                  { lenter; LEXV(BREF, 1, 0); yield(BREF); finally : lcoda; }
<colon_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t :\\]+   { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

 /* semicolon-delimited scanner */
<semicolon_norefs,semicolon_dorefs>[;]                    { lenter; LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<semicolon_norefs,semicolon_dorefs>[;][;]                 { lenter; PUSHSTATE(emptyarg); yyless(1); LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<semicolon_norefs,semicolon_dorefs>[\n\t ]                { lenter; POPSTATE; finally : lcoda; yyless(0); }
<semicolon_norefs,semicolon_dorefs>-?[0-9]+               { lenter; LEXV(I64, 0, 0); yield(I64); finally : lcoda; }

<semicolon_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ;]+   { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

<semicolon_dorefs>\\[\\\n\t ;]                            { lenter; LEX(STR, 1); yield(STR); finally : lcoda; }
<semicolon_dorefs>\\[abtnvfrs]                            { lenter; LEXV(CREF, 1, 0); yield(CREF); finally : lcoda; }
<semicolon_dorefs>\\x[0-9a-fA-F]{1,2}                     { lenter; LEXV(HREF, 2, 0); yield(HREF); finally : lcoda; }
<semicolon_dorefs>\\x\{[0-9a-fA-F]{1,2}\}                 { lenter; LEXV(HREF, 3, 0); yield(HREF); finally : lcoda; }
<semicolon_dorefs>\\[0-9]+                                { lenter; LEXV(BREF, 1, 0); yield(BREF); finally : lcoda; }
<semicolon_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ;\\]+ { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

 /* braces-enclosed scanner */
<braces>[{]                                             { lenter; PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<braces>[\n\t ]                                         { lenter; POPSTATE; finally : lcoda; yyless(0); }
<braces_norefs,braces_dorefs>[{]                        { lenter; PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); LEX(STR, 0); yield(STR); finally : lcoda; }
<braces_norefs,braces_dorefs>[}]                        { lenter; POPSTATE; if(TOPSTATE == braces) { LEX(yytext[0], 0); yield(yytext[0]); } else { LEX(STR, 0); yield(STR); } finally : lcoda; }

<braces_norefs,braces_dorefs>-?[0-9]+                   { lenter; LEXV(I64, 0, 0); yield(I64); finally : lcoda; }

<braces_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF{}]+        { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

<braces_dorefs>\\[}]                                    { lenter; LEX(STR, 1); yield(STR); finally : lcoda; }
<braces_dorefs>\\[abtnvfrs]                             { lenter; LEXV(CREF, 1, 0); yield(CREF); finally : lcoda; }
<braces_dorefs>\\x[0-9a-fA-F]{1,2}                      { lenter; LEXV(HREF, 2, 0); yield(HREF); finally : lcoda; }
<braces_dorefs>\\x\{[0-9a-fA-F]{1,2}\}                  { lenter; LEXV(HREF, 3, 0); yield(HREF); finally : lcoda; }
<braces_dorefs>\\[0-9]+                                 { lenter; LEXV(BREF, 1, 0); yield(BREF); finally : lcoda; }
<braces_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF{}\\]+      { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

 /* brackets-enclosed scanner */
<brackets>[[]                                           { lenter; PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<brackets>[\n\t ]                                       { lenter; POPSTATE; yyless(0); finally : lcoda; }
<brackets_norefs,brackets_dorefs>[[]                    { lenter; PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); LEX(STR, 0); yield(STR); finally : lcoda; }
<brackets_norefs,brackets_dorefs>[\]]                   { lenter; POPSTATE; if(TOPSTATE == brackets) { LEX(yytext[0], 0); yield(yytext[0]); } else { LEX(STR, 0); yield(STR); } finally : lcoda; }

<brackets_norefs,brackets_dorefs>-?[0-9]+               { lenter; LEXV(I64, 0, 0); yield(I64); finally : lcoda; }

<brackets_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF[\]]+     { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

<brackets_dorefs>\\[\]]                                 { lenter; LEX(STR, 1); yield(STR); finally : lcoda; }
<brackets_dorefs>\\[abtnvfrs]                           { lenter; LEXV(CREF, 1, 0); yield(CREF); finally : lcoda; }
<brackets_dorefs>\\x[0-9a-fA-F]{1,2}                    { lenter; LEXV(HREF, 2, 0); yield(HREF); finally : lcoda; }
<brackets_dorefs>\\x\{[0-9a-fA-F]{1,2}\}                { lenter; LEXV(HREF, 3, 0); yield(HREF); finally : lcoda; }
<brackets_dorefs>\\[0-9]+                               { lenter; LEXV(BREF, 1, 0); yield(BREF); finally : lcoda; }
<brackets_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF[\]\\]+   { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

 /* parens-enclosed scanner */
<parens>[(]                                             { lenter; PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<parens>[\n\t ]                                         { lenter; POPSTATE; finally : lcoda; yyless(0); }
<parens_norefs,parens_dorefs>[(]                        { lenter; PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); LEX(STR, 0); yield(STR); finally : lcoda; }
<parens_norefs,parens_dorefs>[)]                        { lenter; POPSTATE; if(TOPSTATE == parens) { LEX(yytext[0], 0); yield(yytext[0]); } else { LEX(STR, 0); yield(STR); } finally : lcoda; }

<parens_norefs,parens_dorefs>-?[0-9]+                   { lenter; LEXV(I64, 0, 0); yield(I64); finally : lcoda; }

<parens_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF()]+        { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

<parens_dorefs>\\[)]                                    { lenter; LEX(STR, 1); yield(STR); finally : lcoda; }
<parens_dorefs>\\[abtnvfrs]                             { lenter; LEXV(CREF, 1, 0); yield(CREF); finally : lcoda; }
<parens_dorefs>\\x[0-9a-fA-F]{1,2}                      { lenter; LEXV(HREF, 2, 0); yield(HREF); finally : lcoda; }
<parens_dorefs>\\x\{[0-9a-fA-F]{1,2}\}                  { lenter; LEXV(HREF, 3, 0); yield(HREF); finally : lcoda; }
<parens_dorefs>\\[0-9]+                                 { lenter; LEXV(BREF, 1, 0); yield(BREF); finally : lcoda; }
<parens_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF()\\]+      { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

 /* angles-enclosed scanner */
<angles>[<]                                             { lenter; PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); LEX(yytext[0], 0); yield(yytext[0]); finally : lcoda; }
<angles>[\n\t ]                                         { lenter; POPSTATE; finally : lcoda; yyless(0); }
<angles_norefs,angles_dorefs>[<]                        { lenter; PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); LEX(STR, 0); yield(STR); finally : lcoda; }
<angles_norefs,angles_dorefs>[>]                        { lenter; POPSTATE; if(TOPSTATE == angles) { LEX(yytext[0], 0); yield(yytext[0]); } else { LEX(STR, 0); yield(STR); } finally : lcoda; }

<angles_norefs,angles_dorefs>-?[0-9]+                   { lenter; LEXV(I64, 0, 0); yield(I64); finally : lcoda; }

<angles_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF<>]+        { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

<angles_dorefs>\\[>]                                    { lenter; LEX(STR, 1); yield(STR); finally : lcoda; }
<angles_dorefs>\\[abtnvfrs]                             { lenter; LEXV(CREF, 1, 0); yield(CREF); finally : lcoda; }
<angles_dorefs>\\x[0-9a-fA-F]{1,2}                      { lenter; LEXV(HREF, 2, 0); yield(HREF); finally : lcoda; }
<angles_dorefs>\\x\{[0-9a-fA-F]{1,2}\}                  { lenter; LEXV(HREF, 3, 0); yield(HREF); finally : lcoda; }
<angles_dorefs>\\[0-9]+                                 { lenter; LEXV(BREF, 1, 0); yield(BREF); finally : lcoda; }
<angles_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF<>\\]+      { lenter; LEX(STR, 0); yield(STR); finally : lcoda; }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          lenter;
                                                          failf(LISTWISE_ILLBYTE, "byte", "0x%02hhx", yytext[0]);
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          failf(LISTWISE_ILLBYTE, "character", "'%1$c'(0x%1$02hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          while(yyextra->states_n)
                                                            { POPSTATE; }
                                                          yield(0); /* return end-of-input */
                                                          finally : lcoda;
                                                        }
