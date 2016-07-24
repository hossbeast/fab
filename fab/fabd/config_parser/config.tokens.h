#ifndef config_TOKENS_H
#define config_TOKENS_H
#define YY_NULL 0
extern int config_numtokens;
extern int * config_tokennumbers;
extern char ** config_tokennames;
enum config_tokennames {
    config_STR = 258,
    config_BOOL = 259,
    config_INT = 260,
    config_HEX = 261,
    config_FLOAT = 262,
    config_CREF = 263,
    config_HREF = 264,
};
#define config_MINTOKEN     258
#define config_MAXTOKEN     264
#endif
