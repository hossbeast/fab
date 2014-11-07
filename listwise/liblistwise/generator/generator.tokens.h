#ifndef generator_TOKENS_H
#define generator_TOKENS_H
#define YY_NULL 0
extern int generator_numtokens;
extern int * generator_tokennumbers;
extern char ** generator_tokennames;
enum generator_tokennames {
     generator_STR = 258,
     generator_LF = 259,
     generator_WS = 260,
     generator_BREF = 261,
     generator_CREF = 262,
     generator_HREF = 263,
     generator_I64 = 264,
     generator_OP = 265,
};
#endif
