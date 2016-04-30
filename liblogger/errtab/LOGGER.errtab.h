#ifndef _LOGGER_ERRTAB_H
#define _LOGGER_ERRTAB_H
#include "xapi.h"
#define ETABLE_LOGGER \
_E(1, TOOMANY, "number of unique categories exceeds the limit") \
_E(2, ILLREPEAT, "category repeated non-consecutively") \
_E(3, ILLORDER, "incompatible ordering of category definitions") \
_E(4, BADFILTER, "invalid filter expression") \
_E(5, NXSTREAM, "stream definition has unknown type") \

enum {
#define _E(a, b, c) LOGGER_ ## b = a,
ETABLE_LOGGER
#undef _E
};
#define ERRMIN_LOGGER 1
#define ERRMAX_LOGGER 5
extern etable * perrtab_LOGGER;
#endif
