#ifndef _MAIN_ERRTAB_H
#define _MAIN_ERRTAB_H
#include "xapi.h"
#define ETABLE_MAIN \
_E(1, BADFILE, "unable to process file") \
_E(2, BADARGS, "invalid command-line arguments") \

enum {
#define _E(a, b, c) MAIN_ ## b = a,
ETABLE_MAIN
#undef _E
};
#define ERRMIN_MAIN 1
#define ERRMAX_MAIN 2
extern etable * perrtab_MAIN;
#endif
