#ifndef cfg_TOKENS_H
#define cfg_TOKENS_H
#define YY_NULL 0
extern int cfg_numtokens;
extern int * cfg_tokennumbers;
extern char ** cfg_tokennames;
enum cfg_tokennames {
    cfg_FILESYSTEM = 258,
    cfg_PATH = 259,
    cfg_INVALIDATION = 260,
    cfg_STAT = 261,
    cfg_CONTENT = 262,
    cfg_NOTIFY = 263,
    cfg_ALWAYS = 264,
    cfg_NEVER = 265,
    cfg_LF = 266,
    cfg_WORD = 267,
    cfg_HREF = 268,
    cfg_CREF = 269,
};
#define cfg_MINTOKEN     258
#define cfg_MAXTOKEN     269
#endif
