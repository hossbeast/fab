#ifndef transform_TOKENS_H
#define transform_TOKENS_H
#define YY_NULL 0
extern int transform_numtokens;
extern int * transform_tokennumbers;
extern char ** transform_tokennames;
enum transform_tokennames {
     transform_STR = 258,
     transform_LF = 259,
     transform_WS = 260,
     transform_BREF = 261,
     transform_CREF = 262,
     transform_HREF = 263,
     transform_I64 = 264,
     transform_OP = 265,
};
#define transform_MINTOKEN      258
#define transform_MAXTOKEN      265
#endif
