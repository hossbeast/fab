#ifndef _LISTWISE_ERRTAB_H
#define _LISTWISE_ERRTAB_H
#include "xapi.h"
#define ETABLE_LISTWISE \
_E(1, SYNTAX, "transform string could not be parsed") \
_E(2, ILLBYTE, "contains illegal byte(s)") \
_E(3, ILLREF, "illegal backreference") \
_E(4, NXOP, "unknown operator") \
_E(5, ARGSNUM, "wrong number of arguments") \
_E(6, ARGSTYPE, "argument of the wrong type") \
_E(7, ARGSDOM, "argument out of expected range") \
_E(8, NOOBJ, "unknown object type referenced") \
_E(9, NOLIST, "lwx operation requested out-of-bounds list index") \
_E(10, NOROW, "lwx operation requested out-of-bounds row index") \
_E(11, ILLOP, "lwx operation cannot be completed") \

#if XAPI_STACKTRACE
#define LISTWISE_SYNTAX perrtab_LISTWISE->v[1 - ERRMIN_LISTWISE].exit
#define LISTWISE_ILLBYTE perrtab_LISTWISE->v[2 - ERRMIN_LISTWISE].exit
#define LISTWISE_ILLREF perrtab_LISTWISE->v[3 - ERRMIN_LISTWISE].exit
#define LISTWISE_NXOP perrtab_LISTWISE->v[4 - ERRMIN_LISTWISE].exit
#define LISTWISE_ARGSNUM perrtab_LISTWISE->v[5 - ERRMIN_LISTWISE].exit
#define LISTWISE_ARGSTYPE perrtab_LISTWISE->v[6 - ERRMIN_LISTWISE].exit
#define LISTWISE_ARGSDOM perrtab_LISTWISE->v[7 - ERRMIN_LISTWISE].exit
#define LISTWISE_NOOBJ perrtab_LISTWISE->v[8 - ERRMIN_LISTWISE].exit
#define LISTWISE_NOLIST perrtab_LISTWISE->v[9 - ERRMIN_LISTWISE].exit
#define LISTWISE_NOROW perrtab_LISTWISE->v[10 - ERRMIN_LISTWISE].exit
#define LISTWISE_ILLOP perrtab_LISTWISE->v[11 - ERRMIN_LISTWISE].exit
#else
enum {
#define _E(a, b, c) LISTWISE_ ## b = a,
ETABLE_LISTWISE
#undef _E
};
#endif
#define ERRMIN_LISTWISE 1
#define ERRMAX_LISTWISE 11
extern errtab * perrtab_LISTWISE;
#endif
