#ifndef _FABD_ERRTAB_H
#define _FABD_ERRTAB_H
#include "xapi.h"
#define ETABLE_FABD \
_E(1, BADIPC, "ipc failure") \

#if XAPI_STACKTRACE
#define FABD_BADIPC perrtab_FABD->v[1 - ERRMIN_FABD].exit
#else
enum {
#define _E(a, b, c) FABD_ ## b = a,
ETABLE_FABD
#undef _E
};
#endif
#define ERRMIN_FABD 1
#define ERRMAX_FABD 1
extern errtab * perrtab_FABD;
#endif
