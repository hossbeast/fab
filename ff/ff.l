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

	#include "ff.parse.h"
	#include "ffn.h"
	#include "ff/ff.tab.h"
	#include "ff/ff.tokens.h"

	#include "log.h"
	#include "macros.h"

	#define TOPSTATE(x) (x->states_n ? x->states[x->states_n-1] : 0)
	#define STATENAME(x)																			\
		  x == INITIAL					? "INITIAL"											\
		: x == quoteword				? "quoteword"										\
		: x == generator				? "generator"										\
		: x == generatorws			? "generatorws"									\
		: x == command					? "command"											\
		: x == commandchecklit	? "commandchecklit"							\
		: x == commandlit				? "commandlit"									\
		: x == list							? "list"												\
		: x == varref						? "varref"											\
		: x == nofile						? "nofile"											\
		: x == multilinecomment	? "multilinecomment" : "UNKNWN"

	#define PUSHSTATE(a, b)																				\
		do {																												\
			log(L_FF | L_FFSTAT, "(%2d) %s:%d -> %s:%d @ %d"					\
				, b->states_n																						\
				, STATENAME(TOPSTATE(b)), TOPSTATE(b), STATENAME(a), a	\
				, __LINE__																							\
			);																												\
			b->states[b->states_n++] = a;															\
			yy_push_state(a, b->scanner);															\
		} while(0)

	#define POPSTATE(b)																							\
		do {																													\
			int x = TOPSTATE(b);																				\
			yy_pop_state(b->scanner);																		\
			b->states_n--;																							\
			log(L_FF | L_FFSTAT, "(%2d) %s:%d <- %s:%d @ %d"						\
				, b->states_n																							\
				, STATENAME(TOPSTATE(b)), TOPSTATE(b), STATENAME(x), x		\
				, __LINE__																								\
			);																													\
		} while(0)

	#define PTOKEN(s, l, t, lc, ln)																						\
		do {																																		\
			if(log_would(L_FF | L_FFTOKN))																				\
			{																																			\
				char buf[256];																											\
				int bufl = stresc(buf, sizeof(buf), s, l);													\
				log(L_FF | L_FFTOKN, "%8s ) '%.*s'%*s @ [%3d,%3d - %3d,%3d] - %d"		\
					, ff_tokname(t)																										\
					, bufl																														\
					, buf																															\
					, MAX(50 - bufl, 0)																								\
					, ""																															\
					, lc->first_line																									\
					, lc->first_column																								\
					, lc->last_line																										\
					, lc->last_column																									\
					, ln																															\
				);																																	\
			}																																			\
		}	while(0)																															\

	#define LEX(token)					\
		lexify(yytext, yyleng, 0, 1, yylval, yylloc, token, yyextra, __LINE__)

	#define LEXX(token, del)		\
		lexify(yytext, yyleng, del, 1, yylval, yylloc, token, yyextra, __LINE__)

	int stresc(char * const buf, size_t sz, const char * const s, int l)
	{
		int o = 0;
		int x;
		for(x = 0; x < l; x++)
		{
			int r = 0;
			if(s[x] == 0x09)
				r = snprintf(buf + o, sz - o, "\\t");
			else if(s[x] == 0x0a)
				r = snprintf(buf + o, sz - o, "\\n");
			else if(s[x] == 0x0d)
				r = snprintf(buf + o, sz - o, "\\r");
			else if(s[x] >= 0x20 && s[x] <= 0x7e)
				r = snprintf(buf + o, sz - o, "%c", s[x]);
			else
				r = snprintf(buf + o, sz - o, "\\x%02hhx", s[x]);

			if(r >= (sz - o))
				break;
			else
				o += r;
		}

		return o;
	}

	void loc_reset(parse_param * xtra, YYLTYPE* lloc)
	{
		xtra->loc->f_lin++;
		xtra->loc->l_lin = xtra->loc->f_lin;

		xtra->loc->f_col = 0;
		xtra->loc->l_col = 0;
	}

	void locify(parse_param* xtra, YYLTYPE* lloc, int leng, int del)
	{
		xtra->loc->f_col = xtra->loc->l_col;
		xtra->loc->l_col = xtra->loc->f_col + leng;

		lloc->first_column = xtra->loc->f_col + del;
		lloc->last_column  = xtra->loc->l_col + del - 1;
		lloc->first_line	 = xtra->loc->f_lin;
		lloc->last_line		 = xtra->loc->l_lin;

		xtra->loc->f_col += del;
		xtra->loc->l_col += del;
	}

	int lexify(const char* text, int leng, int del, int doloc, YYSTYPE* lval, YYLTYPE* lloc, int token, parse_param* xtra, int ln)
	{
		if(doloc)
			locify(xtra, lloc, leng, del);
		PTOKEN(text, leng, token, lloc, ln);

		lval->str.s = text;
		lval->str.e = text + leng;
		lval->num.v = token;

		/* store for error reporting */
		memcpy(xtra->last_loc, lloc, sizeof(*xtra->last_loc));
		xtra->last_tok = token;
		xtra->last_s = lval->str.s;
		xtra->last_e = lval->str.e;

		return token;
	}
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="ff_yy"
%option extra-type="parse_param*"
%option stack

%x quoteword
%x generatorws
%x generator

%x command
%x commandchecklit
%x commandlit

%x blockcheck

%x list

%x varref
%x nofile
%x multilinecomment

%%

<INITIAL,list>[#][^\x00-\x08\x0a-\x1F\x7F-\xFF]*			{
																												// single-line comments
																												locify(yyextra, yylloc, yyleng, 0);
																											}
<INITIAL,list,multilinecomment>[/][*]									{
																												// multi-line comments
																												PUSHSTATE(multilinecomment, yyextra);
																												locify(yyextra, yylloc, yyleng, 0);
																											}
<multilinecomment>[^*\x00-\x08\x0a-\x1F\x7F-\xFF]+		{ locify(yyextra, yylloc, yyleng, 0); }
<multilinecomment>[*][^/\x00-\x08\x0a-\x1F\x7F-\xFF]	{ locify(yyextra, yylloc, yyleng, 0); }
<multilinecomment>[*][/]															{
																												POPSTATE(yyextra);
																												locify(yyextra, yylloc, yyleng, 0);
																											}

<INITIAL,list,multilinecomment>[\n]										{
																												// characters with special meaning in both INITIAL and list
																												loc_reset(yyextra, yylloc);
																											}
<INITIAL,list>[\t ]+																	{ locify(yyextra, yylloc, yyleng, 0); }
<INITIAL,list,command>[[]															{
																												PUSHSTATE(list, yyextra);
																												return LEX(yytext[0]);
																											}
<INITIAL,list>["]																			{ PUSHSTATE(quoteword, yyextra); }
<INITIAL,list>[$]																			{
																												PUSHSTATE(varref, yyextra);
																												return LEX(yytext[0]);
																											}
<INITIAL,list>[@]																			{
																												PUSHSTATE(nofile, yyextra);
																												return LEX(yytext[0]);
																											}
<INITIAL,list>[~]																			{
																												PUSHSTATE(generator, yyextra);
																												return LEX(yytext[0]);
																											}

<INITIAL>[+]																					{
																												// characters with special meaning in INITIAL
																												return LEX(yytext[0]);
																											}
<INITIAL>[:]																					{ return LEX(yytext[0]); }
<INITIAL>[(]																					{ return LEX(yytext[0]); }
<INITIAL>[)]																					{ return LEX(yytext[0]); }
<INITIAL>[-]																					{ return LEX(yytext[0]); }
<INITIAL>[*]																					{ return LEX(yytext[0]); }
<INITIAL>[\^]																					{ return LEX(yytext[0]); }
<INITIAL>[=]																					{ return LEX(yytext[0]); }
<INITIAL>[;]																					{ return LEX(yytext[0]); }
<INITIAL>[%]																					{ return LEX(yytext[0]); }
<INITIAL>[}]																					{ return LEX(yytext[0]); }
<INITIAL>[{]																					{
																												PUSHSTATE(command, yyextra);
																												return LEX(yytext[0]);
																											}
<list>[]]																							{
																												// list-specific
																												POPSTATE(yyextra);
																												return LEX(yytext[0]);
																											}
<list>[,]																							{ return LEX(yytext[0]); }

<INITIAL>"once"																				{
																												PUSHSTATE(blockcheck, yyextra);
																												return LEX(ONCE);
																											}
<INITIAL>[^ \\#"$@~\x00-\x1F\x7F-\xFF/[+:()\-*^=;}{]+	{
																												// string of anything else 
																												return LEX(WORD);
																											}
<list>[^ \\#"$@~\x00-\x1F\x7F-\xFF\],]+ 							{
																												// string of anything else 
																												return LEX(WORD);
																											}
<blockcheck>[\n]																			{ loc_reset(yyextra, yylloc); }
<blockcheck>[\t ]+																		{ locify(yyextra, yylloc, yyleng, 0); }
<blockcheck>[{]																				{
																												POPSTATE(yyextra);
																												return LEX(yytext[0]);
																											}
<blockcheck>[\x00-\xFF]																{ POPSTATE(yyextra); yyless(0); }

<command>\n																						{
																												// command
																												PUSHSTATE(commandchecklit, yyextra);
																												int x = LEX(LF);
																												loc_reset(yyextra, yylloc);
																												return x;
																											}
<command>[}]																					{
																												POPSTATE(yyextra);
																												return LEX(yytext[0]);
																											}
<command>[^[}\x00-\x08\x0a-\x1F\x7F-\xFF]+						{ return LEX(WORD); }
<commandchecklit>\t+>[\t ]+														{
																												POPSTATE(yyextra);
																												PUSHSTATE(commandlit, yyextra);
																												locify(yyextra, yylloc, yyleng, 0);
																											}
<commandchecklit>[\x00-\xff]													{ POPSTATE(yyextra); yyless(0); }
<commandlit>[^\x00-\x08\x0a-\x1F\x7F-\xFF]+						{
																												POPSTATE(yyextra);
																												return LEX(WORD);
																											}
<varref>[@*#<]																				{
																												POPSTATE(yyextra);
																												return LEX(WORD);
																											}
<varref>[a-zA-Z_][a-zA-Z0-9_]*												{
																												POPSTATE(yyextra);
																												return LEX(WORD);
																											}
<nofile>[a-zA-Z_][a-zA-Z0-9_]*												{ return LEX(WORD); }
<nofile>[.]																						{ return LEX(yytext[0]); }
<nofile>[\x00-\xFF]																		{ POPSTATE(yyextra); yyless(0); }

<generator>[ \t]+																			{ locify(yyextra, yylloc, yyleng, 0); }
<generator>[\n]																				{ loc_reset(yyextra, yylloc); }
<generator>["]																				{
																												POPSTATE(yyextra);
																												PUSHSTATE(quoteword, yyextra);
																												locify(yyextra, yylloc, yyleng, 0);
																											}
<generator>[[]																				{
																												POPSTATE(yyextra);
																												PUSHSTATE(list, yyextra);
																												return LEX(yytext[0]);
																											}
<generator>[^ \t\n"[]																	{
																												POPSTATE(yyextra);
																												PUSHSTATE(generatorws, yyextra);
																												yyless(0);
																											}
<generatorws>[^\x00-\x1F\x20]+												{ return LEX(GWORD); }
<generatorws>[ \t\n]																	{ POPSTATE(yyextra); yyless(0); }
<quoteword>\\\\																				{ yytext++; yyleng--; return LEXX(QWORD, 1); }
<quoteword>\\["]																			{ yytext++; yyleng--; return LEXX(QWORD, 1); }
<quoteword>[^\\"\x00-\x1F\x7F-\xFF]+									{ return LEX(QWORD); }
<quoteword>["]																				{ POPSTATE(yyextra); locify(yyextra, yylloc, yyleng, 0); }

<*>[\x00]																							{
																												while(yyextra->states_n)
																													POPSTATE(yyextra);
																												return 0;
																											}
<*>[\x01-\x1F\x7F-\xFF]																{
																												char space[64];
																												snprintf(space, sizeof(space), "unknown character : (0x%02hhx)", yytext[0]);
																												ff_yyerror(0, 0, yyextra, space);
																												return 0;
																											}
<*>[\x20-\x7E]																				{
																												char space[64];
																												snprintf(space, sizeof(space), "unknown character : '%1$c'(0x%1$02hhx)", yytext[0]);
																												ff_yyerror(0, 0, yyextra, space);
																												return 0;
																											}
