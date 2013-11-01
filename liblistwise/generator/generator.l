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
	#include "generator/genscan.h"

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
			, __LINE__								\
			, 0												\
		)

	#define LEXS(token, ldel)			\
		lexify(											\
			  token										\
			, yylval									\
			, yylloc									\
			, yyextra									\
			, yytext									\
			, yyleng									\
			, ldel										\
			, 0												\
			, __LINE__								\
			, 0												\
		)

	#define LEXC(token, vdel)			\
		lexify(											\
			  token										\
			, yylval									\
			, yylloc									\
			, yyextra									\
			, yytext									\
			, yyleng									\
			, 0												\
			, vdel										\
			, __LINE__								\
			, 0												\
		)

	#define LEXV(token, vdel, v)	\
		lexify(											\
			  token										\
			, yylval									\
			, yylloc									\
			, yyextra									\
			, yytext									\
			, yyleng									\
			, 0												\
			, vdel										\
			, __LINE__								\
			, v												\
		)

	int lexify(const int token, YYSTYPE * const lval, yyu_location * const lloc, parse_param * const xtra, char * const text, const int leng, const int locdel, const int valdel, const int line, void * const v)
	{
		// populate lexical value if any
		if(token == CREF)
		{
			if(text[valdel] == 'a')
				lval->ref = 0x07;
			else if(text[valdel] == 'b')
				lval->ref = 0x08;
			else if(text[valdel] == 't')
				lval->ref = 0x09;
			else if(text[valdel] == 'n')
				lval->ref = 0x0a;
			else if(text[valdel] == 'v')
				lval->ref = 0x0b;
			else if(text[valdel] == 'f')
				lval->ref = 0x0c;
			else if(text[valdel] == 'r')
				lval->ref = 0x0d;
		}
		else if(token == BREF)
			sscanf(text + valdel, "%d", &lval->ref);
		else if(token == HREF)
			sscanf(text + valdel, "%x", &lval->ref);
		else if(token == I64)
			sscanf(text + valdel, "%"SCNd64, &lval->i64);
		else if(token == OP)
			lval->op = *(operator **)v;

		return yyu_lexify(token, lval, sizeof(*lval), lloc, &xtra->yyu_extra, text, leng, locdel, token == LF, line);
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

<INITIAL,ws,multilinecomment>[/][*]											{ LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]																{ LOCWRITE; POPSTATE; }
<multilinecomment>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+		{ LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]	{ LOCWRITE; }
<multilinecomment>[/]																		{ LOCWRITE; }
<multilinecomment>[\n]																	{ LOCRESET; }

<INITIAL,ws>[\n]																				{ return LEX(LF); }
<INITIAL,ws>[\t ]+																			{ return LEX(WS); }
<INITIAL,ws>[-a-zA-Z]+																	{
																													typeof(*yylval->op) * op;
				 																									if((op = op_lookup(yytext, yyleng)))
																													{
				 																										return LEXV(OP, 0, &op);
																													}
																													else if(TOPSTATE == INITIAL)
																													{
																														PUSHSTATE(genscan_argstate[yyextra->scanmode]);
																														return LEX(STR);
																													}
																													else
																													{
																														snprintf(yyextra->temp, sizeof(yyextra->temp), "unknown operator : %s", yytext);
																														generator_yyerror(0, 0, yyextra, yyextra->temp);
																														return 0;
																													}
																												}
<INITIAL,ws>[!][/{}\\,]+																{ yyextra->scanmode = genscan_parse(yytext, yyleng); }
<INITIAL,ws>[^\x00-\x08\x0B-\x1F\x7F-\xFF]							{ PUSHSTATE(genscan_basestate[yyextra->scanmode]); yyless(0); }

<slash>[^\x00-\x08\x0B-\x1F\x7F-\xFF/]									{ POPSTATE; yyless(0); if(GS_REFS(yyextra->scanmode)) { PUSHSTATE(slash_arg_withrefs); } else { PUSHSTATE(slash_arg_norefs); } }

<slash_arg_norefs>[\n\t /]															{ POPSTATE; yyless(0); }
<slash_arg_norefs>-?[0-9]+															{ return LEXV(I64, 0, 0); }
<slash_arg_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /]+	{ return LEX(STR); }

<slash_arg_withrefs>\\[/]																			{ return LEXS(STR, 1); }
<slash_arg_withrefs>\\[\\]																		{ return LEXS(STR, 1); }
<slash_arg_withrefs>\\[ ]																			{ return LEXS(STR, 1); }
<slash_arg_withrefs>\\[\n]																		{ return LEXC(CREF, 1); }
<slash_arg_withrefs>\\[\r]																		{ return LEXC(CREF, 1); }
<slash_arg_withrefs>\\[\b]																		{ return LEXC(CREF, 1); }
<slash_arg_withrefs>\\[\t]																		{ return LEXC(CREF, 1); }
<slash_arg_withrefs>\\[\f]																		{ return LEXC(CREF, 1); }
<slash_arg_withrefs>\\[\a]																		{ return LEXC(CREF, 1); }
<slash_arg_withrefs>\\[\v]																		{ return LEXC(CREF, 1); }
<slash_arg_withrefs>\\x[0-9a-fA-F]{1,2}												{ return LEXV(HREF, 2, 0); }
<slash_arg_withrefs>\\x\{[0-9a-fA-F]{1,2}\}										{ return LEXV(HREF, 3, 0); }
<slash_arg_withrefs>\\[0-9]+																	{ return LEXV(BREF, 1, 0); }
<slash_arg_withrefs>[\n\t /]																	{ POPSTATE; yyless(0); }
<slash_arg_withrefs>-?[0-9]+																	{ return LEXV(I64, 0, 0); }
<slash_arg_withrefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /\\]+		{ return LEX(STR); }

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
