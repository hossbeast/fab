#ifndef _LORIEN_ERRTAB_H
#define _LORIEN_ERRTAB_H
#include "xapi.h"
#define ETABLE_LORIEN \
_E(1, CYCLE, "dependency graph contains a cycle") \

enum {
#define _E(a, b, c) LORIEN_ ## b = a,
ETABLE_LORIEN
#undef _E
};
#define ERRMIN_LORIEN 1
#define ERRMAX_LORIEN 1
extern etable * perrtab_LORIEN;
#endif
