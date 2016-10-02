#ifndef _MAIN_ERRTAB_H
#define _MAIN_ERRTAB_H
#include "xapi.h"
#define ETABLE_MAIN \
_E(1, BADARGS, "invalid command-line arguments") \
_E(2, NXSWITCH, "unknown switch in program arguments") \
_E(3, NOTPATH, "object argument is not a path") \

#if XAPI_STACKTRACE
#define MAIN_BADARGS perrtab_MAIN->v[1 - ERRMIN_MAIN].exit
#define MAIN_NXSWITCH perrtab_MAIN->v[2 - ERRMIN_MAIN].exit
#define MAIN_NOTPATH perrtab_MAIN->v[3 - ERRMIN_MAIN].exit
#else
enum {
#define _E(a, b, c) MAIN_ ## b = a,
ETABLE_MAIN
#undef _E
};
#endif
#define ERRMIN_MAIN 1
#define ERRMAX_MAIN 3
extern errtab * perrtab_MAIN;
#endif
