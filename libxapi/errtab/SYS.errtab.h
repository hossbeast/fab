#ifndef _SYS_ERRTAB_H
#define _SYS_ERRTAB_H
#include "xapi.h"
#define ETABLE_SYS \
_E(1, BADARGS, "failed to parse command-line arguments") \
_E(2, AUNLOAD, "asymmetric load-unload sequence") \
_E(3, NXPARAMS, "unable to determine runtime parameters") \
_E(4, NOTSUPP, "operation not supported") \
_E(5, INVALID, "invalid argument") \

#if XAPI_STACKTRACE
#define SYS_BADARGS perrtab_SYS->v[1 - ERRMIN_SYS].exit
#define SYS_AUNLOAD perrtab_SYS->v[2 - ERRMIN_SYS].exit
#define SYS_NXPARAMS perrtab_SYS->v[3 - ERRMIN_SYS].exit
#define SYS_NOTSUPP perrtab_SYS->v[4 - ERRMIN_SYS].exit
#define SYS_INVALID perrtab_SYS->v[5 - ERRMIN_SYS].exit
#else
enum {
#define _E(a, b, c) SYS_ ## b = a,
ETABLE_SYS
#undef _E
};
#endif
#define ERRMIN_SYS 1
#define ERRMAX_SYS 5
extern errtab * perrtab_SYS;
#endif
