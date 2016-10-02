#ifndef _VALUE_ERRTAB_H
#define _VALUE_ERRTAB_H
#include "xapi.h"
#define ETABLE_VALUE \
_E(1, DIFFTYPE, "cant merge values of differing type") \
_E(2, BADTYPE, "merge trees must have aggregate type") \

#if XAPI_STACKTRACE
#define VALUE_DIFFTYPE perrtab_VALUE->v[1 - ERRMIN_VALUE].exit
#define VALUE_BADTYPE perrtab_VALUE->v[2 - ERRMIN_VALUE].exit
#else
enum {
#define _E(a, b, c) VALUE_ ## b = a,
ETABLE_VALUE
#undef _E
};
#endif
#define ERRMIN_VALUE 1
#define ERRMAX_VALUE 2
extern errtab * perrtab_VALUE;
#endif
