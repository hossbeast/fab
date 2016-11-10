#ifndef _MAIN_ERRTAB_H
#define _MAIN_ERRTAB_H
#include "xapi.h"
#define ETABLE_MAIN \
_E(1, BADARGS, "failed to parse command-line arguments") \

#if XAPI_STACKTRACE
#define MAIN_BADARGS perrtab_MAIN->v[1 - ERRMIN_MAIN].exit
#else
enum {
#define _E(a, b, c) MAIN_ ## b = a,
ETABLE_MAIN
#undef _E
};
#endif
#define ERRMIN_MAIN 1
#define ERRMAX_MAIN 1
extern errtab * perrtab_MAIN;
#endif