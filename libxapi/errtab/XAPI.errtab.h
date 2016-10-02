#ifndef _XAPI_ERRTAB_H
#define _XAPI_ERRTAB_H
#include "xapi.h"
#define ETABLE_XAPI \
_E(1, ILLFATAL, "non-xapi function invoked with fatal") \
_E(2, NOFATAL, "xapi function invoked without fatal") \
_E(3, NOCODE, "fail invoked without a code") \
_E(4, NOTABLE, "fail invoked without a table") \
_E(5, NOMEM, "out of memory (not calltree related)") \

#if XAPI_STACKTRACE
#define XAPI_ILLFATAL perrtab_XAPI->v[1 - ERRMIN_XAPI].exit
#define XAPI_NOFATAL perrtab_XAPI->v[2 - ERRMIN_XAPI].exit
#define XAPI_NOCODE perrtab_XAPI->v[3 - ERRMIN_XAPI].exit
#define XAPI_NOTABLE perrtab_XAPI->v[4 - ERRMIN_XAPI].exit
#define XAPI_NOMEM perrtab_XAPI->v[5 - ERRMIN_XAPI].exit
#else
enum {
#define _E(a, b, c) XAPI_ ## b = a,
ETABLE_XAPI
#undef _E
};
#endif
#define ERRMIN_XAPI 1
#define ERRMAX_XAPI 5
extern errtab * perrtab_XAPI;
#endif
