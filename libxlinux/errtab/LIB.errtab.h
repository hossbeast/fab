#ifndef _LIB_ERRTAB_H
#define _LIB_ERRTAB_H
#include "xapi.h"
#define ETABLE_LIB \
_E(1, AUNLOAD, "asymmetric load-unload sequence") \

enum {
#define _E(a, b, c) LIB_ ## b = a,
ETABLE_LIB
#undef _E
};
#define ERRMIN_LIB 1
#define ERRMAX_LIB 1
extern etable * perrtab_LIB;
#endif
