/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

	#include "global.h"

	#include "ff.parse.h"
	#include "ffn.h"
	#include "ff/ff.tab.h"
	#include "ff/ff.tokens.h"

	#include "logs.h"
	#include "macros.h"

	// contains much of the implementation of the scanner
	#include "yyutil.h"

	/// LEX
	//
	// SUMMARY
	//  parse a whitespace or character token with no semantic value possibly at an offset from yytext
	// 
	// PARAMETERS
	//  token - token to parse
	//  ldel  - offset at which the location for this token begins 
	//
	#define LEX(token, ldel)			\
		lexify(											\
			  token										\
			, yylval									\
			, yylloc									\
			, yyextra									\
			, yytext									\
			, yyleng									\
			, ldel										\
			, 0												\
			, __FUNCTION__						\
			, __FILE__								\
			, __LINE__								\
			, 0												\
		)

	int lexify(const int token, YYSTYPE * const lval, yyu_location * const lloc, parse_param * const xtra, char * const text, const int leng, const int ldel, const int vdel, const char * func, const char * file, int line, void * const v)
	{
		return yyu_lexify(token, lval, sizeof(*lval), lloc, &xtra->yyu_extra, text, leng, ldel, token == LF, func, file, line);
	}
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="ff_yy"
%option extra-type="parse_param*"
%option stack

%x quoteword
%x transform

%x command
%x commandchecklit
%x commandlit

%x blockcheck

%x list

%x varref
%x nofile
%x multilinecomment

/* bytes that cannot appear in the input */
ILL		[\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* single-line comments */
<INITIAL,list>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*			{ LOCWRITE; }

 /* multiline comments are nestable */
<INITIAL,list,multilinecomment>[/][*]										{ LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]																{ LOCWRITE; POPSTATE; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+	{ LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]	{ LOCWRITE; }
<multilinecomment>[/]																		{ LOCWRITE; }
<multilinecomment>[\n]																	{ LOCRESET; }

 /* characters with special meaning in both INITIAL and list */
<INITIAL,list>[\n]																			{ LOCRESET; }
<INITIAL,list>[\t ]+																		{ LOCWRITE; }
<INITIAL,list,command>[[]																{ PUSHSTATE(list); return LEX(yytext[0], 0); }
<INITIAL,list>["]																				{ PUSHSTATE(quoteword); }
<INITIAL,list>[$]																				{ PUSHSTATE(varref); return LEX(yytext[0], 0); }
<INITIAL,list>[@]																				{	PUSHSTATE(nofile); return LEX(yytext[0], 0); }
<INITIAL,list>[~][\t ]*																	{ PUSHSTATE(transform); return LEX(yytext[0], 0); }

 /* characters with special meaning in INITIAL */
<INITIAL>[+]																						{ return LEX(yytext[0], 0); }
<INITIAL>[:]																						{ return LEX(yytext[0], 0); }
<INITIAL>[(]																						{ return LEX(yytext[0], 0); }
<INITIAL>[)]																						{ return LEX(yytext[0], 0); }
<INITIAL>[-]																						{ return LEX(yytext[0], 0); }
<INITIAL>[*]																						{ return LEX(yytext[0], 0); }
<INITIAL>[\^]																						{ return LEX(yytext[0], 0); }
<INITIAL>[=]																						{ return LEX(yytext[0], 0); }
<INITIAL>[;]																						{ return LEX(yytext[0], 0); }
<INITIAL>[%]																						{ return LEX(yytext[0], 0); }
<INITIAL>[}]																						{ return LEX(yytext[0], 0); }
<INITIAL>[{]																						{ PUSHSTATE(command); return LEX(yytext[0], 0); }
<INITIAL>"once"																					{ PUSHSTATE(blockcheck); return LEX(ONCE, 0); }
<INITIAL>[^ \\#"$@~\x00-\x1F\x7F-\xFF/[+:()\-*^=;}{]+		{	return LEX(WORD, 0); }

 /* list-specific */
<list>[]]																								{ POPSTATE; return LEX(yytext[0], 0); }
<list>[^ \\#"$@~\x00-\x1F\x7F-\xFF[\]]+ 								{ return LEX(WORD, 0); }

<blockcheck>[\n]																				{ LOCRESET; }
<blockcheck>[\t ]+																			{ LOCWRITE; }
<blockcheck>[{]																					{	POPSTATE; return LEX(yytext[0], 0); }
<blockcheck>[\x00-\xFF]																	{ POPSTATE; yyless(0); }

 /* command */
<command>[\n]																						{ PUSHSTATE(commandchecklit); return LEX(LF, 0); }
<command>[}]																						{	POPSTATE; return LEX(yytext[0], 0); }
<command>[^[}\x00-\x08\x0a-\x1F\x7F-\xFF]+							{ return LEX(WORD, 0); }
<commandchecklit>[\t ]+>[\t ]+													{
																													POPSTATE;
																													PUSHSTATE(commandlit);
																													LOCWRITE;
																												}
<commandchecklit>[\x00-\xff]														{ POPSTATE; yyless(0); }
<commandlit>[^\x00-\x08\x0a-\x1F\x7F-\xFF]+							{	POPSTATE;	return LEX(WORD, 0); }

<varref>[@*#<!]																					{ POPSTATE; return LEX(WORD, 0); }
<varref>[a-zA-Z_][a-zA-Z0-9_]*													{ POPSTATE; return LEX(WORD, 0); }

<nofile>[a-zA-Z_][a-zA-Z0-9_]*													{ return LEX(WORD, 0); }
<nofile>[.]																							{ return LEX(yytext[0], 0); }
<nofile>[\x00-\xFF]																			{ POPSTATE; yyless(0); }

<transform>[ \t]+																				{ return LEX(WORD, 0); }
<transform>[\n]																					{ return LEX(LF, 0); }
<transform>[]]																					{ POPSTATE; yyless(0); }
<transform>[[]																					{	POPSTATE;	PUSHSTATE(list); return LEX(yytext[0], 0); }
<transform>[^ \x00-\x1F\x7F-\xFF[\]]+										{ return LEX(WORD, 0); }
<quoteword>\\\\																					{ yytext++; yyleng--; return LEX(WORD, 1); }
<quoteword>\\["]																				{ yytext++; yyleng--; return LEX(WORD, 1); }
<quoteword>[^\\"\x00-\x1F\x7F-\xFF]+										{ return LEX(WORD, 0); }
<quoteword>["]																					{ POPSTATE; LOCWRITE; }

<*>[\x00-\x1F\x7F-\xFF]																	{
																													while(yyextra->states_n)
																														POPSTATE;
																													LOCWRITE;
																													yyu_scanner_error(yylloc, yyextra, FAB_ILLBYTE, "unexpected byte 0x%02hhx", yytext[0]);
																													return 0;	/* return end-of-input */
																												}
<*>[\x20-\x7E]																					{
																													while(yyextra->states_n)
																														POPSTATE;
																													LOCWRITE;
																													yyu_scanner_error(yylloc, yyextra, FAB_ILLBYTE, "unexpected character '%c'", yytext[0]);
																													return 0; /* return end-of-input */
																												}
<<EOF>>                                                 {
                                                          while(yyextra->states_n)
                                                            POPSTATE;
                                                          return 0; /* return end-of-input */
                                                        }
