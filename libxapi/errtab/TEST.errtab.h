#ifndef _TEST_ERRTAB_H
#define _TEST_ERRTAB_H
#include "xapi.h"
#define ETABLE_TEST \
_E(1, ERROR_ONE, "testing error in libxapi unit tests") \
_E(2, ERROR_TWO, "testing error in libxapi unit tests") \

enum {
#define _E(a, b, c) TEST_ ## b = a,
ETABLE_TEST
#undef _E
};
#define ERRMIN_TEST 1
#define ERRMAX_TEST 2
extern etable * perrtab_TEST;
#endif
