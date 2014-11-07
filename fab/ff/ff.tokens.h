#ifndef ff_TOKENS_H
#define ff_TOKENS_H
#define YY_NULL 0
extern int ff_numtokens;
extern int * ff_tokennumbers;
extern char ** ff_tokennames;
enum ff_tokennames {
     ff_WORD = 258,
     ff_WS = 259,
     ff_LF = 260,
     ff_ONCE = 261,
};
#endif
