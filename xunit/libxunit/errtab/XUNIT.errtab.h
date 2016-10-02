#ifndef _XUNIT_ERRTAB_H
#define _XUNIT_ERRTAB_H
#include "xapi.h"
#define ETABLE_XUNIT \
_E(1, FAIL, "test failure") \

#if XAPI_STACKTRACE
#define XUNIT_FAIL perrtab_XUNIT->v[1 - ERRMIN_XUNIT].exit
#else
enum {
#define _E(a, b, c) XUNIT_ ## b = a,
ETABLE_XUNIT
#undef _E
};
#endif
#define ERRMIN_XUNIT 1
#define ERRMAX_XUNIT 1
extern errtab * perrtab_XUNIT;
#endif
