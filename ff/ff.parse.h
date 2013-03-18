#ifndef FF_PARSE_H
#define FF_PARSE_H

typedef struct
{
	struct ff_file *		ff;					// fabfile being parsed
	struct ff_node *		ffn;				// ffn being parsed
	void *							scanner;		// scanner

	int									states[64];	// start states stack
	int									states_n;

	struct ff_loc *			loc;				// running location track for this parse

	struct ff_loc *			last_loc;		// state pertaining to last parsed token
	int									last_tok;
	const char*					last_s;
	const char*					last_e;

	const char*					orig_base;	// ptr to original input string
	int									orig_len;		// length of original input string
	char*								act_base;		// ptr to lexer copy

	int									r;					// zeroed in yyerror
} parse_param;


// defined in ff.tab.o (this declaration should be in ff.tab.h but isn't)
int ff_yyparse(void *, parse_param*);

// defined in ff.dsc.tab.o (this declaration should be in ff.dsc.tab.h but isn't)
int ff_dsc_yyparse(void *, parse_param*);

/// ff_yyerror
//
// SUMMARY
//  flag the parse as failed (by setting pp->r = 0)
//  log the error under L_ERROR | L_FF
//  log the last token under L_ERROR | L_FF
//
// DETAILS
//  called from tab.o and lex.o
//
void ff_yyerror(void * loc, void* scanner, parse_param* pp, char const* err);

#endif
