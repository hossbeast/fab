#ifndef _TEST_ERRTAB_H
#define _TEST_ERRTAB_H
#include "xapi.h"
#define ETABLE_TEST \
_E(1, ERROR_ONE, "testing error in libxapi unit tests") \
_E(2, ERROR_TWO, "testing error in libxapi unit tests") \

#if XAPI_STACKTRACE
#define TEST_ERROR_ONE perrtab_TEST->v[1 - ERRMIN_TEST].exit
#define TEST_ERROR_TWO perrtab_TEST->v[2 - ERRMIN_TEST].exit
#else
enum {
#define _E(a, b, c) TEST_ ## b = a,
ETABLE_TEST
#undef _E
};
#endif
#define ERRMIN_TEST 1
#define ERRMAX_TEST 2
extern errtab * perrtab_TEST;
#endif
