%{
	#include <stdio.h>

	#include "ff.h"
	#include "ff.tab.h"
	#include "ff.tokens.h"

	#include "log.h"

	#define MIN(a,b)            \
	 ({ typeof (a) _a = (a);    \
			 typeof (b) _b = (b);   \
		 _a > _b ? _b : _a; })


	#define PTOKEN(s, l, t, lc, ln)																				\
			log(L_FF | L_FFTOK, "%15s ) %.*s%*s @ [%3d,%3d - %3d,%3d] - %d"		\
				, ff_tokname(t)																									\
				, MIN(t == WS ? 0 : l, 50)																			\
				, t == WS ? "" : s																							\
				, 50 - MIN(t == WS ? 0 : l, 50)																	\
				, ""																														\
				, lc->first_line																								\
				, lc->first_column																							\
				, lc->last_line																									\
				, lc->last_column																								\
				, ln)

	#define LEX(token)					\
		lexify(yytext, yyleng, yylval, yylloc, token, 0, 0, yyextra, __LINE__)

	#define LEXV(token, vs, vl)	\
		lexify(yytext, yyleng, yylval, yylloc, token, vs, vs + vl, yyextra, __LINE__)

	void loc_reset(YYLTYPE* lloc)
	{
		lloc->first_line++;
		lloc->last_line = lloc->first_line;

		lloc->first_column = 0;
		lloc->last_column = 0;
	}

	void locify(YYLTYPE* lloc, int leng)
	{
		lloc->first_column = lloc->last_column;
		lloc->last_column  = lloc->first_column + leng;
	}

	int lexify(const char* text, int leng, YYSTYPE* lval, YYLTYPE* lloc, int token, char* vs, char* ve, parse_param* xtra, int ln)
	{
		locify(lloc, leng);
		PTOKEN(text, leng, token, lloc, ln);

		lval->str.s = text;
		lval->str.e = text + leng;
		lval->num.v = token;
		lval->var.vs = vs;
		lval->var.ve = ve;

		return token;
	}
%}

%option nounput noyywrap
%option reentrant
%option bison-bridge bison-locations
%option prefix="ff_yy"
%option extra-type="parse_param*"
%option stack

%x command
%x quoteword
%x generatorws
%x generator

%%

[#].*												{ locify(yylloc, yyleng); }

>>													{ return LEX(INC); }
=>>													{
															yy_push_state(generatorws, yyextra->scanner);
															return LEX(LW);
														}

[^\n\t\\ :"$[\]{=\x00]+			{ return LEX(WORD); }
\\\\												{ yytext++; yyleng--; return LEX(WORD); }
\\[ ]												{ yytext++; yyleng--; return LEX(WORD); }
\\[$]												{ yytext++; yyleng--; return LEX(WORD); }
\\[:]												{ yytext++; yyleng--; return LEX(WORD); }
\\[[]												{ yytext++; yyleng--; return LEX(WORD); }
\\[]]												{ yytext++; yyleng--; return LEX(WORD); }
\\["]												{ yytext++; yyleng--; return LEX(WORD); }
\\[{]												{ yytext++; yyleng--; return LEX(WORD); }
\\[=]												{ yytext++; yyleng--; return LEX(WORD); }

\n													{ loc_reset(yylloc); }
[\t ]+											{ locify(yylloc, yyleng); }
\$[a-zA-Z_@*][a-zA-Z0-9_]*		{ return LEXV(VARNAME, yytext + 1, yyleng - 1); }
\$\{[a-zA-Z_@*][a-zA-Z0-9_]*\}	{ return LEXV(VARNAME, yytext + 2, yyleng - 3); }
[:]													{ return LEX(yytext[0]); }
[[]													{ return LEX(yytext[0]); }
[]]													{ return LEX(yytext[0]); }
[=]													{ return LEX(yytext[0]); }
["]													{
															yy_push_state(quoteword, yyextra->scanner);
															return LEX(yytext[0]);
														}
[{]													{
															yy_push_state(command, yyextra->scanner);
															return LEX(yytext[0]);
														}
<generatorws>{
[\t ]+["]										{
															yy_push_state(quoteword, yyextra->scanner);
															return LEX('"');
														}
[\t ]+											{
															yy_push_state(generator, yyextra->scanner);
															return LEX(WS);
														}

.														{ locify(yylloc, yyleng); ff_yyerror(0, yyextra->scanner, yyextra, "unknown character"); return 0; }
}
<generator>{
[\n\t ]											{
															yy_pop_state(yyextra->scanner);
															yy_pop_state(yyextra->scanner);
															return LEX(WS);
														}
\\\\												{ yytext++; yyleng--; return LEX(WORD); }
\\[ ]+											{ yytext++; yyleng--; return LEX(WORD); }
[^\\\n\t ]+									{ return LEX(WORD); }

.														{ locify(yylloc, yyleng); ff_yyerror(0, yyextra->scanner, yyextra, "unknown character"); return 0; }
}
<quoteword>{
["]													{
															yy_pop_state(yyextra->scanner);
															return LEX(yytext[0]);
														}
\\\\												{ yytext++; yyleng--; return LEX(WORD); }
\\["]												{ yytext++; yyleng--; return LEX(WORD); }
[^\\"]+											{ return LEX(WORD); }

.														{ locify(yylloc, yyleng); ff_yyerror(0, yyextra->scanner, yyextra, "unknown character"); return 0; }
}
<command>{
[}]													{
															yy_pop_state(yyextra->scanner);
															return LEX(yytext[0]);
														}
=>>													{
															yy_push_state(generatorws, yyextra->scanner);
															return LEX(LW);
														}
[\t ]+											{ return LEX(WS); }
\n													{ loc_reset(yylloc); return LEX(WS); }

[^\n\t\\ \$[\]\x00]+				{ return LEX(WORD); }
\\\\												{ yytext++; yyleng--; return LEX(WORD); }
\\[$]												{ yytext++; yyleng--; return LEX(WORD); }
\\[[]												{ yytext++; yyleng--; return LEX(WORD); }
\\[]]												{ yytext++; yyleng--; return LEX(WORD); }

\$[a-zA-Z_@*][a-zA-Z0-9_]*		{ return LEX(VARNAME); }
\$\{[a-zA-Z_@*][a-zA-Z0-9_]*\}	{ return LEX(VARNAME); }
[[]													{ return LEX(yytext[0]); }
[]]													{ return LEX(yytext[0]); }

.														{ locify(yylloc, yyleng); ff_yyerror(0, yyextra->scanner, yyextra, "unknown character"); return 0; }
}

\x00												{ return END; }
.														{ locify(yylloc, yyleng); ff_yyerror(0, yyextra->scanner, yyextra, "unknown character"); return 0; }
