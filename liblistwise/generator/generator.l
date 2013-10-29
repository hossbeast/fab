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
	#include <inttypes.h>

	#include "generator/generator.def.h"
	#include "generator/generator.tab.h"
	#include "generator/generator.tokens.h"

	#include "macros.h"

	/// YYUSAY
	//
	// write informational messages to listwise_info_fd
	//
	#define YYUSAY(...) dprintf(listwise_info_fd, ##__VA_ARGS__)

	/// YYULVAL
	//
	// write textual description of a token/lval to a buffer
	//
	#define YYULVAL(token, lval, xtra, bbuf, blen)										\
		bbuf = xtra->space2;																						\
		blen = 0;																												\
		if(token == I64)																								\
			blen = snprintf(bbuf, sizeof(bbuf), "%"PRId64, lval->i64);		\
		else if(token == BREF)																					\
			blen = snprintf(bbuf, sizeof(bbuf), "%d", lval->bref);				\
																																										
	#include "yyutil.h"

	#define LEX(token, del, v)	\
		lexify(										\
			  token									\
			, yylval								\
			, yylloc								\
			, yyextra								\
			, yytext								\
			, yyleng								\
			, del										\
			, __LINE__							\
			, token == LF						\
			, token == BREF					\
			, token == I64					\
			, token == OP						\
			, v											\
		)

	int lexify(const int token, YYSTYPE * const lval, yyu_location * const lloc, parse_param * const xtra, char * const text, const int leng, const int del, const int line, const int isnl, const int isbref, const int isi64, const int isop, void * const v)
	{
		// populate lexical value if any
		if(isbref)
			sscanf(text + del, "%d", &lval->bref);
		else if(isi64)
			sscanf(text + del, "%"SCNd64, &lval->i64);
		else if(isop)
			lval->op = *(operator **)v;

		return yyu_lexify(token, lval, sizeof(*lval), lloc, &xtra->yyu_extra, text, leng, del, isnl, line);
	}
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="generator_yy"
%option extra-type="parse_param*"
%option stack

%x next
%x ws 
%x operator
%x slash
%x argument
%x multilinecomment

ILL		[\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

<INITIAL,multilinecomment>[/][*]												{ LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]																{ LOCWRITE; POPSTATE; }
<multilinecomment>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+		{ LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]	{ LOCWRITE; }
<multilinecomment>[/]																		{ LOCWRITE; }
<INITIAL,multilinecomment>[\n]													{ LOCRESET; }

<INITIAL>[\t ]+																					{ return LEX(WS, 0, 0); }
<INITIAL>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /]+					{
																													PUSHSTATE(next);
																													typeof(*yylval->op) * op;

				 																									if((op = op_lookup(yytext, yyleng)))
																														PUSHSTATE(operator);
																													else
																														PUSHSTATE(argument);
																													yyless(0);
																												}
<next>[\n]																							{ PUSHSTATE(ws); return LEX(LF, 0, 0); }
<next>[\t ]+																						{ PUSHSTATE(ws); return LEX(WS, 0, 0); }
<next>[/]																								{ PUSHSTATE(slash); return LEX(yytext[0], 0, 0); }
<ws>[\n]																								{ return LEX(LF, 0, 0); }
<ws>[\t ]+																							{ return LEX(WS, 0, 0); }
<ws>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ]+								{ POPSTATE; PUSHSTATE(operator); yyless(0); }
<operator>[-a-zA-Z]+																		{
																													POPSTATE;

																													typeof(*yylval->op) * op;
				 																									if((op = op_lookup(yytext, yyleng)))
																													{
				 																										return LEX(OP, 0, &op);
																													}
																													else
																													{
																														snprintf(yyextra->temp, sizeof(yyextra->temp), "unknown operator : %s", yytext);
																														generator_yyerror(0, 0, yyextra, yyextra->temp);
																														return 0;
																													}
																												}
<slash>[/]																							{ POPSTATE; yyless(0); return LEX(STR, 1, 0); }
<slash>[^\x00-\x08\x0B-\x1F\x7F-\xFF/]+									{ POPSTATE; PUSHSTATE(argument); yyless(0); }
<argument>\\[0-9]+																			{ return LEX(BREF, 1, 0); }
<argument>\\[/]																					{ return LEX(STR, 1, 0); }
<argument>\\[\t ]+																			{ return LEX(STR, 1, 0); }
<argument>-?[0-9]+																			{ return LEX(I64, 0, 0); }
<argument>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /\\]+				{ return LEX(STR, 0, 0); }
<argument>[\n\t /]																			{ POPSTATE; yyless(0); }
<*>[\x00]																								{
																													while(yyextra->states_n)
																													{
																														POPSTATE;
																													}
																													return 0;
																												}
<*>[\x01-\x1F\x7F-\xFF]																	{
																													snprintf(yyextra->temp, sizeof(yyextra->temp), "unknown character : (0x%02hhx)", yytext[0]);
																													generator_yyerror(0, 0, yyextra, yyextra->temp);
																													return 0;
																												}
<*>[\x20-\x7E]																					{
																													snprintf(yyextra->temp, sizeof(yyextra->temp), "unknown character : '%1$c'(0x%1$02hhx)", yytext[0]);
																													generator_yyerror(0, 0, yyextra, yyextra->temp);
																													return 0;
																												}
