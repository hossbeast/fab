#ifndef _VALUE_ERRTAB_H
#define _VALUE_ERRTAB_H
#include "xapi.h"
#define ETABLE_VALUE \
_E(1, DIFFTYPE, "cant merge differently typed values") \

#if XAPI_STACKTRACE
#define VALUE_DIFFTYPE perrtab_VALUE->v[1 - ERRMIN_VALUE].exit
#else
enum {
#define _E(a, b, c) VALUE_ ## b = a,
ETABLE_VALUE
#undef _E
};
#endif
#define ERRMIN_VALUE 1
#define ERRMAX_VALUE 1
extern errtab * perrtab_VALUE;
#endif
