#ifndef _LW_ERRTAB_H
#define _LW_ERRTAB_H
#include "xapi.h"
#define ETABLE_LW \
_E(1, SYNTAX, "transform string could not be parsed") \
_E(2, ILLBYTE, "transform string contains illegal byte(s)") \
_E(3, ILLREF, "transform contains illegal backreference") \
_E(4, NXOP, "transform contains unknown operator") \
_E(5, ARGSNUM, "transform wrong number of arguments") \
_E(6, ARGSTYPE, "transform argument of the wrong type") \
_E(7, ARGSDOM, "transform argument out of expected range") \
_E(8, NOOBJ, "unknown object type referenced") \
_E(9, NOLIST, "lwx operation requested out-of-bounds list index") \
_E(10, NOROW, "lwx operation requested out-of-bounds row index") \
_E(11, ILLOP, "lwx operation cannot be completed") \

enum {
#define _E(a, b, c) LW_ ## b = a,
ETABLE_LW
#undef _E
};
#define ERRMIN_LW 1
#define ERRMAX_LW 11
extern etable * perrtab_LW;
#endif