#ifndef _SYS_ERRTAB_H
#define _SYS_ERRTAB_H
#include "xapi.h"
#define ETABLE_SYS \
_E(1, BADARGS, "failed to parse command-line arguments") \
_E(2, AUNLOAD, "asymmetric load-unload sequence") \
_E(3, NXPARAMS, "unable to determine runtime parameters") \

enum {
#define _E(a, b, c) SYS_ ## b = a,
ETABLE_SYS
#undef _E
};
#define ERRMIN_SYS 1
#define ERRMAX_SYS 3
extern etable * perrtab_SYS;
#endif
