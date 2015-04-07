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

	#include "global.h"

	#include "cfg.parse.h"
	#include "cfg/cfg.tab.h"
	#include "cfg/cfg.tokens.h"

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
	//
	#define LEX(token)						\
		lexify(											\
			  token										\
			, yylval									\
			, yylloc									\
			, yyextra									\
			, yytext									\
			, yyleng									\
			, 0												\
			, 0												\
			, __FUNCTION__						\
			, __FILE__								\
			, __LINE__								\
			, 0												\
		)

	#define LEXV(token, ldel)			\
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

%x quoted
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

(?:filesystem)																					{ return LEX(FILESYSTEM); }
(?:invalidation-method)																	{ return LEX(INVALIDATION_METHOD); }
(?:path)																								{ return LEX(PATH); }
(?:stat)																								{ return LEX(STAT); }
(?:content)																							{ return LEX(CONTENT); }
(?:notify)																							{ return LEX(NOTIFY); }
(?:valid)																								{ return LEX(VALID); }
(?:invalid)																							{ return LEX(INVALID); }

 /* characters with special meaning in both INITIAL and list */
[\n]																										{ LOCRESET; return LEX(yytext[0]); }
[\t ]+																									{ LOCWRITE; }
["]																											{ PUSHSTATE(quoted); return LEX(yytext[0]); }

 /* characters with special meaning in INITIAL */
[:]																											{ return LEX(yytext[0]); }
[}]																											{ return LEX(yytext[0]); }
[{]																											{ return LEX(yytext[0]); }
[^\n\t ":}{\x00-\x1F\x7F-\xFF]+													{	return LEX(WORD); }

<quoted>["]																							{ POPSTATE; return LEX(yytext[0]); }
<quoted>\\[abtnvfrs0]																		{ return LEXV(CREF, 1); }
<quoted>\\x[0-9a-fA-F]{2}																{ return LEXV(HREF, 2); }
<quoted>\\x\{[0-9a-fA-F]{1,2}\}													{ return LEXV(HREF, 3); }
<quoted>[^\n"\x00-\x1F\x7F-\xFF]+												{ return LEX(WORD); }

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
