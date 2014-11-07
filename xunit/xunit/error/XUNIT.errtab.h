#ifndef _XUNIT_ERRTAB_H
#define _XUNIT_ERRTAB_H
#include "xapi.h"
#define ETABLE_XUNIT \
_E(1, BADARGS, "invalid command-line arguments") \

enum {
#define _E(a, b, c) XUNIT_ ## b = a,
ETABLE_XUNIT
#undef _E
};
#define ERRMIN_XUNIT 1
#define ERRMAX_XUNIT 1
extern etable * perrtab_XUNIT;
#endif
