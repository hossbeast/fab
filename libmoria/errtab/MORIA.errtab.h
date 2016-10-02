#ifndef _MORIA_ERRTAB_H
#define _MORIA_ERRTAB_H
#include "xapi.h"
#define ETABLE_MORIA \
_E(1, CYCLE, "graph contains a cycle") \

#if XAPI_STACKTRACE
#define MORIA_CYCLE perrtab_MORIA->v[1 - ERRMIN_MORIA].exit
#else
enum {
#define _E(a, b, c) MORIA_ ## b = a,
ETABLE_MORIA
#undef _E
};
#endif
#define ERRMIN_MORIA 1
#define ERRMAX_MORIA 1
extern errtab * perrtab_MORIA;
#endif
