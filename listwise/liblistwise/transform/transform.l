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

	#include "transform/transform.def.h"
	#include "transform/transform.tab.h"
	#include "transform/transform.tokens.h"
	#include "genscan.internal.h"
  #include "operators.internal.h"

	#include "macros.h"

	// contains much of the implemention of the scanner
	#include "yyutil.h"

	/// LEX
	//
	// SUMMARY
	//  parse a whitespace or character token with no semantic value at an offset from yytext
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
			, __FUNCTION__						\
			, __FILE__								\
			, __LINE__								\
			, v												\
		)

	/// lexify
	//
	// SUMMARY
	//  populate semantic value, invoke yyu_lexify
	//
	int lexify(const int token, YYSTYPE * const lval, yyu_location * const lloc, parse_param * const xtra, char * const text, const int leng, const int ldel, const int vdel, const char * func, const char * file, const int line, void * const v)
	{
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

		return yyu_lexify(token, lval, sizeof(*lval), lloc, &xtra->yyu_extra, text, leng, ldel, token == LF, func, file, line);
	}
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="transform_yy"
%option extra-type="parse_param*"
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
ILL		[\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* single-line comments */
<INITIAL,ws>[#][^\x00-\x08\x0A-\x1F\x7F-\xFF]*					{ LOCWRITE; }

 /* multiline comments are nestable */
<INITIAL,ws,multilinecomment>[/][*]											{ LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]																{ LOCWRITE; POPSTATE; }
<multilinecomment>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+		{ LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]	{ LOCWRITE; }
<multilinecomment>[/]																		{ LOCWRITE; }
<multilinecomment>[\n]																	{ LOCRESET; }

 /*
 ** In the INITIAL state, check whether a string is an operator name. If not, leave it to be scanned by
 ** the current scanmode. In either case, push ws so that we only get back to the INITIAL state when
 ** processing a scanmode directive
 **
 */

<INITIAL,ws>[\n]																				{ return LEX(LF, 0); }
<INITIAL,ws>[\t ]+																			{ return LEX(WS, 0); }

<INITIAL>[-a-zA-Z]+																			{
																													PUSHSTATE(ws);
																													typeof(*yylval->op) * op;
																													if((op = op_lookup(yytext, yyleng)))
																													{
																														return LEXV(OP, 0, &op);
																													}
																													else
																													{
																														PUSHSTATE(genscan_startcondition_initial[yyextra->scanmode]);
																														yyless(0);
																													}
																												}
<INITIAL>[^-\x00-\x08\x0B-\x1F\x7F-\xFFa-zA-Z\n\t ?]+		{
																													PUSHSTATE(ws);
																													PUSHSTATE(genscan_startcondition_initial[yyextra->scanmode]);
																													yyless(0);
																												}

 /* following ws, an alpha string must be an operator name or mnemonic */
<ws>[-a-zA-Z]+																					{
																													typeof(*yylval->op) * op;
																													if((op = op_lookup(yytext, yyleng)))
																													{
																														return LEXV(OP, 0, &op);
																													}
																													else
																													{
																														while(yyextra->states_n)
																															POPSTATE;
																														LOCWRITE;
																														yyu_scanner_error(yylloc, yyextra, LW_NXOP, "unknown operator : %s", yytext);
																														return 0;
																													}
																												}
 /* after processing a scanmode directive, revert to the INITIAL scanner */
<INITIAL>[?]{1,2}[/,.:;{[(<]														{ LOCWRITE; yyextra->scanmode = genscan_parse(yytext, yyleng); }
<ws>[\t ]*[?]{1,2}[/,.:;{[(<][\t ]*											{ LOCWRITE; yyextra->scanmode = genscan_parse(yytext, yyleng); POPSTATE; }

 /* process anything else with the current scanmode */
<ws>[^\x00-\x08\x0B-\x1F\x7F-\xFF]											{ PUSHSTATE(genscan_startcondition_initial[yyextra->scanmode]); yyless(0); }

 /* fabricate an empty-string STR token */
<emptyarg>[\x00-\xff]																		{ POPSTATE; yyless(0); return LEX(STR, 1); }

 /* slash-delimited scanner */
<slash_norefs,slash_dorefs>[/]													{ return LEX(yytext[0], 0); }
<slash_norefs,slash_dorefs>[/][/]												{ PUSHSTATE(emptyarg); yyless(1); return LEX(yytext[0], 0); }
<slash_norefs,slash_dorefs>[\n\t ]											{ POPSTATE; yyless(0); }
<slash_norefs,slash_dorefs>-?[0-9]+											{ return LEXV(I64, 0, 0); }

<slash_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /]+			{ return LEX(STR, 0); }

<slash_dorefs>\\[\\\n\t /]															{ return LEX(STR, 1); }
<slash_dorefs>\\[abtnvfrs]															{ return LEXV(CREF, 1, 0); }
<slash_dorefs>\\x[0-9a-fA-F]{1,2}												{ return LEXV(HREF, 2, 0); }
<slash_dorefs>\\x\{[0-9a-fA-F]{1,2}\}										{ return LEXV(HREF, 3, 0); }
<slash_dorefs>\\[0-9]+																	{ return LEXV(BREF, 1, 0); }
<slash_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /\\]+		{ return LEX(STR, 0); }

 /* comma-delimited scanner */
<comma_norefs,comma_dorefs>[,]													{ return LEX(yytext[0], 0); }
<comma_norefs,comma_dorefs>[,][,]												{ PUSHSTATE(emptyarg); yyless(1); return LEX(yytext[0], 0); }
<comma_norefs,comma_dorefs>[\n\t ]											{ POPSTATE; yyless(0); }
<comma_norefs,comma_dorefs>-?[0-9]+											{ return LEXV(I64, 0, 0); }

<comma_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ,]+			{ return LEX(STR, 0); }

<comma_dorefs>\\[\\\n\t ,]															{ return LEX(STR, 1); }
<comma_dorefs>\\[abtnvfrs]															{ return LEXV(CREF, 1, 0); }
<comma_dorefs>\\x[0-9a-fA-F]{1,2}												{ return LEXV(HREF, 2, 0); }
<comma_dorefs>\\x\{[0-9a-fA-F]{1,2}\}										{ return LEXV(HREF, 3, 0); }
<comma_dorefs>\\[0-9]+																	{ return LEXV(BREF, 1, 0); }
<comma_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ,\\]+		{ return LEX(STR, 0); }

 /* dot-delimited scanner */
<dot_norefs,dot_dorefs>[.]															{ return LEX(yytext[0], 0); }
<dot_norefs,dot_dorefs>[.][.]														{ PUSHSTATE(emptyarg); yyless(1); return LEX(yytext[0], 0); }
<dot_norefs,dot_dorefs>[\n\t ]													{ POPSTATE; yyless(0); }
<dot_norefs,dot_dorefs>-?[0-9]+													{ return LEXV(I64, 0, 0); }

<dot_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t .]+				{ return LEX(STR, 0); }

<dot_dorefs>\\[\\\n\t .]																{ return LEX(STR, 1); }
<dot_dorefs>\\[abtnvfrs]																{ return LEXV(CREF, 1, 0); }
<dot_dorefs>\\x[0-9a-fA-F]{1,2}													{ return LEXV(HREF, 2, 0); }
<dot_dorefs>\\x\{[0-9a-fA-F]{1,2}\}											{ return LEXV(HREF, 3, 0); }
<dot_dorefs>\\[0-9]+																		{ return LEXV(BREF, 1, 0); }
<dot_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t .\\]+			{ return LEX(STR, 0); }

 /* colon-delimited scanner */
<colon_norefs,colon_dorefs>[:]													{ return LEX(yytext[0], 0); }
<colon_norefs,colon_dorefs>[:][:]												{ PUSHSTATE(emptyarg); yyless(1); return LEX(yytext[0], 0); }
<colon_norefs,colon_dorefs>[\n\t ]											{ POPSTATE; yyless(0); }
<colon_norefs,colon_dorefs>-?[0-9]+											{ return LEXV(I64, 0, 0); }

<colon_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t :]+			{ return LEX(STR, 0); }

<colon_dorefs>\\[\\\n\t :]															{ return LEX(STR, 1); }
<colon_dorefs>\\[abtnvfrs]															{ return LEXV(CREF, 1, 0); }
<colon_dorefs>\\x[0-9a-fA-F]{1,2}												{ return LEXV(HREF, 2, 0); }
<colon_dorefs>\\x\{[0-9a-fA-F]{1,2}\}										{ return LEXV(HREF, 3, 0); }
<colon_dorefs>\\[0-9]+																	{ return LEXV(BREF, 1, 0); }
<colon_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t :\\]+		{ return LEX(STR, 0); }

 /* semicolon-delimited scanner */
<semicolon_norefs,semicolon_dorefs>[;]										{ return LEX(yytext[0], 0); }
<semicolon_norefs,semicolon_dorefs>[;][;]									{ PUSHSTATE(emptyarg); yyless(1); return LEX(yytext[0], 0); }
<semicolon_norefs,semicolon_dorefs>[\n\t ]								{ POPSTATE; yyless(0); }
<semicolon_norefs,semicolon_dorefs>-?[0-9]+								{ return LEXV(I64, 0, 0); }

<semicolon_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ;]+		{ return LEX(STR, 0); }

<semicolon_dorefs>\\[\\\n\t ;]														{ return LEX(STR, 1); }
<semicolon_dorefs>\\[abtnvfrs]														{ return LEXV(CREF, 1, 0); }
<semicolon_dorefs>\\x[0-9a-fA-F]{1,2}											{ return LEXV(HREF, 2, 0); }
<semicolon_dorefs>\\x\{[0-9a-fA-F]{1,2}\}									{ return LEXV(HREF, 3, 0); }
<semicolon_dorefs>\\[0-9]+																{ return LEXV(BREF, 1, 0); }
<semicolon_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ;\\]+	{ return LEX(STR, 0); }

 /* braces-enclosed scanner */
<braces>[{]																							{ PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); return LEX(yytext[0], 0); }
<braces>[\n\t ]																					{ POPSTATE; yyless(0); }
<braces_norefs,braces_dorefs>[{]												{ PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); return LEX(STR, 0); }
<braces_norefs,braces_dorefs>[}]												{ POPSTATE; if(TOPSTATE == braces) { return LEX(yytext[0], 0); } else { return LEX(STR, 0); } }

<braces_norefs,braces_dorefs>-?[0-9]+										{ return LEXV(I64, 0, 0); }

<braces_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF{}]+				{ return LEX(STR, 0); }

<braces_dorefs>\\[}]																		{ return LEX(STR, 1); }
<braces_dorefs>\\[abtnvfrs]															{ return LEXV(CREF, 1, 0); }
<braces_dorefs>\\x[0-9a-fA-F]{1,2}											{ return LEXV(HREF, 2, 0); }
<braces_dorefs>\\x\{[0-9a-fA-F]{1,2}\}									{ return LEXV(HREF, 3, 0); }
<braces_dorefs>\\[0-9]+																	{ return LEXV(BREF, 1, 0); }
<braces_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF{}\\]+			{ return LEX(STR, 0); }

 /* brackets-enclosed scanner */
<brackets>[[]																						{ PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); return LEX(yytext[0], 0); }
<brackets>[\n\t ]																				{ POPSTATE; yyless(0); }
<brackets_norefs,brackets_dorefs>[[]										{ PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); return LEX(STR, 0); }
<brackets_norefs,brackets_dorefs>[\]]										{ POPSTATE; if(TOPSTATE == brackets) { return LEX(yytext[0], 0); } else { return LEX(STR, 0); } }

<brackets_norefs,brackets_dorefs>-?[0-9]+								{ return LEXV(I64, 0, 0); }

<brackets_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF[\]]+			{ return LEX(STR, 0); }

<brackets_dorefs>\\[\]]																	{ return LEX(STR, 1); }
<brackets_dorefs>\\[abtnvfrs]														{ return LEXV(CREF, 1, 0); }
<brackets_dorefs>\\x[0-9a-fA-F]{1,2}										{ return LEXV(HREF, 2, 0); }
<brackets_dorefs>\\x\{[0-9a-fA-F]{1,2}\}								{ return LEXV(HREF, 3, 0); }
<brackets_dorefs>\\[0-9]+																{ return LEXV(BREF, 1, 0); }
<brackets_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF[\]\\]+		{ return LEX(STR, 0); }

 /* parens-enclosed scanner */
<parens>[(]																							{ PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); return LEX(yytext[0], 0); }
<parens>[\n\t ]																					{ POPSTATE; yyless(0); }
<parens_norefs,parens_dorefs>[(]												{ PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); return LEX(STR, 0); }
<parens_norefs,parens_dorefs>[)]												{ POPSTATE; if(TOPSTATE == parens) { return LEX(yytext[0], 0); } else { return LEX(STR, 0); } }

<parens_norefs,parens_dorefs>-?[0-9]+										{ return LEXV(I64, 0, 0); }

<parens_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF()]+				{ return LEX(STR, 0); }

<parens_dorefs>\\[)]																		{ return LEX(STR, 1); }
<parens_dorefs>\\[abtnvfrs]															{ return LEXV(CREF, 1, 0); }
<parens_dorefs>\\x[0-9a-fA-F]{1,2}											{ return LEXV(HREF, 2, 0); }
<parens_dorefs>\\x\{[0-9a-fA-F]{1,2}\}									{ return LEXV(HREF, 3, 0); }
<parens_dorefs>\\[0-9]+																	{ return LEXV(BREF, 1, 0); }
<parens_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF()\\]+			{ return LEX(STR, 0); }

 /* angles-enclosed scanner */
<angles>[<]																							{ PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); return LEX(yytext[0], 0); }
<angles>[\n\t ]																					{ POPSTATE; yyless(0); }
<angles_norefs,angles_dorefs>[<]												{ PUSHSTATE(genscan_startcondition_argscan[yyextra->scanmode]); return LEX(STR, 0); }
<angles_norefs,angles_dorefs>[>]												{ POPSTATE; if(TOPSTATE == angles) { return LEX(yytext[0], 0); } else { return LEX(STR, 0); } }

<angles_norefs,angles_dorefs>-?[0-9]+										{ return LEXV(I64, 0, 0); }

<angles_norefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF<>]+				{ return LEX(STR, 0); }

<angles_dorefs>\\[>]																		{ return LEX(STR, 1); }
<angles_dorefs>\\[abtnvfrs]															{ return LEXV(CREF, 1, 0); }
<angles_dorefs>\\x[0-9a-fA-F]{1,2}											{ return LEXV(HREF, 2, 0); }
<angles_dorefs>\\x\{[0-9a-fA-F]{1,2}\}									{ return LEXV(HREF, 3, 0); }
<angles_dorefs>\\[0-9]+																	{ return LEXV(BREF, 1, 0); }
<angles_dorefs>[^\x00-\x08\x0B-\x1F\x7F-\xFF<>\\]+			{ return LEX(STR, 0); }

<*>[\x00-\x1F\x7F-\xFF]																	{
																													while(yyextra->states_n)
																														POPSTATE;
																													LOCWRITE;
																													yyu_scanner_error(yylloc, yyextra, LW_ILLBYTE, "unexpected byte 0x%02hhx", yytext[0]);
																													return 0;	/* return end-of-input */
																												}
<*>[\x20-\x7E]																					{
																													while(yyextra->states_n)
																														POPSTATE;
																													LOCWRITE;
																													yyu_scanner_error(yylloc, yyextra, LW_ILLBYTE, "unexpected character '%1$c'(0x%1$02hhx)", yytext[0]);
																													return 0;	/* return end-of-input */
																												}
<<EOF>>																									{
																													while(yyextra->states_n)
																														POPSTATE;
																													return 0;	/* return end-of-input */
																												}
