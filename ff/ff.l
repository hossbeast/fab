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

	#define MKPTR(x, y) ({								\
		if(!y->act_base)										\
		{																		\
			y->act_base = x;									\
		}																		\
																				\
		y->orig_base + (x - y->act_base);		\
	})

	#define PTOKEN(s, l, t, lc, ln)																			\
			log(L_FF | L_FFTOK, "%15s ) %.*s%*s @ [%3d,%3d %3d,%3d] - %d"		\
				, ff_tokname(t)																								\
				, MIN(l, 50)																									\
				, s																														\
				, 50 - MIN(l, 50)																							\
				, ""																													\
				, lc->first_line																							\
				, lc->first_column																						\
				, lc->last_line																								\
				, lc->last_column																							\
				, ln)

	#define LEX(token)	lexify(MKPTR(yytext, yyextra), yyleng, yylval, yylloc, token, yyextra, __LINE__)

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

	int lexify(const char* text, int leng, YYSTYPE* lval, YYLTYPE* lloc, int token, parse_param* xtra, int ln)
	{
		locify(lloc, leng);
		PTOKEN(text, leng, token, lloc, ln);

		lval->str.s = text;
		lval->str.e = text + leng;
		lval->num.v = token;

		return token;
	}
%}

%option nounput noyywrap
%option reentrant
%option bison-bridge bison-locations
%option prefix="ff_yy"
%option extra-type="parse_param*"

%x command

%%


\n													{ loc_reset(yylloc); PTOKEN(yytext, 0, LF, yylloc, __LINE__); return LEX(LF); }
[ \t]+											{ locify(yylloc, yyleng); }

[:]													{ return LEX(yytext[0]); }
[a-zA-Z_][a-zA-Z0-9_.-/]*		{ return LEX(FNAME); }


"{"													{
															BEGIN(command);
															return LEX(yytext[0]);
														}
<command>{
\$@\.path										{ return LEX(FF_REF_PATH); }
\$@\.edepso									{ return LEX(FF_REF_EDEPSO); }
\$@\.idepsc									{ return LEX(FF_REF_IDEPSC); }
"}"													{
															BEGIN(0);
															return LEX(yytext[0]);
														}
[^$\x0a]*										{ return LEX(TEXT); }
\n													{ loc_reset(yylloc); PTOKEN(yytext, 0, LF, yylloc, __LINE__); return LEX(LF); }
}
\x00												{ return END; }
.														{ locify(yylloc, yyleng); ff_yyerror(0, yyextra->scanner, yyextra, "unknown character"); return 0; }
