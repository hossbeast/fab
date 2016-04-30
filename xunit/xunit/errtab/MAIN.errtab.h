#ifndef _MAIN_ERRTAB_H
#define _MAIN_ERRTAB_H
#include "xapi.h"
#define ETABLE_MAIN \
_E(1, BADARGS, "invalid command-line arguments") \
_E(2, NXSWITCH, "unknown switch in program arguments") \
_E(3, NOTPATH, "object argument is not a path") \

enum {
#define _E(a, b, c) MAIN_ ## b = a,
ETABLE_MAIN
#undef _E
};
#define ERRMIN_MAIN 1
#define ERRMAX_MAIN 3
extern etable * perrtab_MAIN;
#endif
