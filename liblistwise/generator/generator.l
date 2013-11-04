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

%x ws
%x multilinecomment
%x emptyarg
%x slash_norefs slash_dorefs
%x comma_norefs comma_dorefs
%x braces braces_norefs braces_dorefs

/* bytes that cannot appear in the string being scanned */
ILL		[\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* multiline comments are nestable */
<INITIAL,ws,multilinecomment>[/][*]											{ LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]																{ LOCWRITE; POPSTATE; }
<multilinecomment>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+		{ LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]	{ LOCWRITE; }
<multilinecomment>[/]																		{ LOCWRITE; }
<multilinecomment>[\n]																	{ LOCRESET; }

<INITIAL,ws>[\n]																				{ return LEX(LF); }
<INITIAL,ws>[\t ]+																			{ return LEX(WS); }
<INITIAL>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t !]+					{
																													PUSHSTATE(ws);
																													typeof(*yylval->op) * op;
																													if((op = op_lookup(yytext, yyleng)))
																													{
																														return LEXV(OP, 0, &op);
																													}
																													else
																													{
																														PUSHSTATE(genscan_basestate[yyextra->scanmode]);
																														yyless(0);
																													}
																												}
<ws>[-a-zA-Z]+																					{
																													typeof(*yylval->op) * op;
																													if((op = op_lookup(yytext, yyleng)))
																													{
																														return LEXV(OP, 0, &op);
																													}
																													else
																													{
																														snprintf(yyextra->temp, sizeof(yyextra->temp), "unknown operator : %s", yytext);
																														generator_yyerror(0, 0, yyextra, yyextra->temp);
																														return 0;
																													}
																												}
<INITIAL,ws>!{1,2}[/{,]																	{ LOCWRITE; yyextra->scanmode = genscan_parse(yytext, yyleng); }
<ws>[^\x00-\x08\x0B-\x1F\x7F-\xFF]											{ PUSHSTATE(genscan_basestate[yyextra->scanmode]); yyless(0); }

 /* fabricate an empty-string STR token */
<emptyarg>[\x00-\xff]																					{ POPSTATE; yyless(0); return LEXS(STR, 1); }

 /* slash-delimited scanner */
<slash_norefs,slash_dorefs>[/]													{ return LEX(yytext[0]); }
<slash_norefs,slash_dorefs>[/][/]												{ PUSHSTATE(emptyarg); yyless(1); return LEX(yytext[0]); }
<slash_norefs,slash_dorefs>[/][\t ]+!{1,2}[/{,][\t ]+		{ LOCWRITE; POPSTATE; yyextra->scanmode = genscan_parse(yytext, yyleng); }
<slash_norefs,slash_dorefs>[\n\t ]											{ POPSTATE; yyless(0); }
<slash_norefs,slash_dorefs>-?[0-9]+											{ return LEXV(I64, 0, 0); }

<slash_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /]+			{ return LEX(STR); }

<slash_dorefs>\\[\\\n\t /]															{ return LEXS(STR, 1); }
<slash_dorefs>\\[abtnvfr]																{ return LEXC(CREF, 1); }
<slash_dorefs>\\x[0-9a-fA-F]{1,2}												{ return LEXV(HREF, 2, 0); }
<slash_dorefs>\\x\{[0-9a-fA-F]{1,2}\}										{ return LEXV(HREF, 3, 0); }
<slash_dorefs>\\[0-9]+																	{ return LEXV(BREF, 1, 0); }
<slash_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /\\]+		{ return LEX(STR); }

 /* comma-delimited scanner */
<comma_norefs,comma_dorefs>[,]													{ return LEX(yytext[0]); }
<comma_norefs,comma_dorefs>[,][,]												{ PUSHSTATE(emptyarg); yyless(1); return LEX(yytext[0]); }
<comma_norefs,comma_dorefs>[,][\t ]+!{1,2}[/{,][\t ]+		{ LOCWRITE; POPSTATE; yyextra->scanmode = genscan_parse(yytext, yyleng); }
<comma_norefs,comma_dorefs>[\n\t ]											{ POPSTATE; yyless(0); }
<comma_norefs,comma_dorefs>-?[0-9]+											{ return LEXV(I64, 0, 0); }

<comma_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ,]+			{ return LEX(STR); }

<comma_dorefs>\\[\\\n\t ,]															{ return LEXS(STR, 1); }
<comma_dorefs>\\[abtnvfr]																{ return LEXC(CREF, 1); }
<comma_dorefs>\\x[0-9a-fA-F]{1,2}												{ return LEXV(HREF, 2, 0); }
<comma_dorefs>\\x\{[0-9a-fA-F]{1,2}\}										{ return LEXV(HREF, 3, 0); }
<comma_dorefs>\\[0-9]+																	{ return LEXV(BREF, 1, 0); }
<comma_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ,\\]+		{ return LEX(STR); }

 /* braces-enclosed scanner */
<braces>[{]																							{ PUSHSTATE(genscan_argstate[yyextra->scanmode]); return LEX(yytext[0]); }
<braces>[}]																							{ POPSTATE; return LEX(yytext[0]); }
<braces>[\n\t ]																					{ POPSTATE; yyless(0); }
<braces>[\t ]+!{1,2}[/{,][\t ]+													{ LOCWRITE; POPSTATE; yyextra->scanmode = genscan_parse(yytext, yyleng); }
<braces_norefs,braces_dorefs>[{]												{ PUSHSTATE(genscan_argstate[yyextra->scanmode]); return LEX(STR); }
<braces_norefs,braces_dorefs>[}]												{ POPSTATE; return LEX(STR); }

<braces_norefs,braces_dorefs>-?[0-9]+										{ return LEXV(I64, 0, 0); }

<braces_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF{}]+				{ return LEX(STR); }

<braces_dorefs>\\[\\}]																	{ return LEXS(STR, 1); }
<braces_dorefs>\\[abtnvfr]															{ return LEXS(STR, 1); }
<braces_dorefs>\\x[0-9a-fA-F]{1,2}											{ return LEXV(HREF, 2, 0); }
<braces_dorefs>\\x\{[0-9a-fA-F]{1,2}\}									{ return LEXV(HREF, 3, 0); }
<braces_dorefs>\\[0-9]+																	{ return LEXV(BREF, 1, 0); }
<braces_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF{}\\]+			{ return LEX(STR); }

<*>[\x00-\x1F\x7F-\xFF]																	{
																													snprintf(yyextra->temp, sizeof(yyextra->temp), "unknown character : (0x%02hhx)", yytext[0]);
																													generator_yyerror(0, 0, yyextra, yyextra->temp);
																													return 0;
																												}
<*>[\x20-\x7E]																					{
																													snprintf(yyextra->temp, sizeof(yyextra->temp), "unknown character : '%1$c'(0x%1$02hhx)", yytext[0]);
																													generator_yyerror(0, 0, yyextra, yyextra->temp);
																													return 0;
																												}
