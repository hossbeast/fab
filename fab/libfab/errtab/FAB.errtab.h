#ifndef _FAB_ERRTAB_H
#define _FAB_ERRTAB_H
#include "xapi.h"
#define ETABLE_FAB \
_E(1, BADIPC, "ipc failure") \
_E(2, FABPERMS, "fab executables must be owned by fabsys:fabsys and have ug+s permissions") \
_E(3, FABLOCK, "fab already running for this hash") \
_E(4, FABDFAIL, "fabd terminated abnormally") \
_E(5, FABDEXIT, "fabd terminated unexpectedly") \

#if XAPI_STACKTRACE
#define FAB_BADIPC perrtab_FAB->v[1 - ERRMIN_FAB].exit
#define FAB_FABPERMS perrtab_FAB->v[2 - ERRMIN_FAB].exit
#define FAB_FABLOCK perrtab_FAB->v[3 - ERRMIN_FAB].exit
#define FAB_FABDFAIL perrtab_FAB->v[4 - ERRMIN_FAB].exit
#define FAB_FABDEXIT perrtab_FAB->v[5 - ERRMIN_FAB].exit
#else
enum {
#define _E(a, b, c) FAB_ ## b = a,
ETABLE_FAB
#undef _E
};
#endif
#define ERRMIN_FAB 1
#define ERRMAX_FAB 5
extern errtab * perrtab_FAB;
#endif
