#ifndef cfg_TOKENS_H
#define cfg_TOKENS_H
#define YY_NULL 0
extern int cfg_numtokens;
extern int * cfg_tokennumbers;
extern char ** cfg_tokennames;
enum cfg_tokennames {
     cfg_FILESYSTEM = 258,
     cfg_PATH = 259,
     cfg_INVALIDATION_METHOD = 260,
     cfg_STAT = 261,
     cfg_CONTENT = 262,
     cfg_INOTIFY = 263,
     cfg_LF = 264,
     cfg_WORD = 265,
};
#endif
