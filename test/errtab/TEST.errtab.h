#ifndef _TEST_ERRTAB_H
#define _TEST_ERRTAB_H
#include "xapi.h"
#define ETABLE_TEST \
_E(1, FAIL, "failed assertion") \

#if XAPI_STACKTRACE
#define TEST_FAIL perrtab_TEST->v[1 - ERRMIN_TEST].exit
#else
enum {
#define _E(a, b, c) TEST_ ## b = a,
ETABLE_TEST
#undef _E
};
#endif
#define ERRMIN_TEST 1
#define ERRMAX_TEST 1
extern errtab * perrtab_TEST;
#endif
