#ifndef _MORIA_ERRTAB_H
#define _MORIA_ERRTAB_H
#include "xapi.h"
#define ETABLE_MORIA \
_E(1, CYCLE, "graph contains a cycle") \

enum {
#define _E(a, b, c) MORIA_ ## b = a,
ETABLE_MORIA
#undef _E
};
#define ERRMIN_MORIA 1
#define ERRMAX_MORIA 1
extern etable * perrtab_MORIA;
#endif
