#ifndef _LOGGER_ERRTAB_H
#define _LOGGER_ERRTAB_H
#include "xapi.h"
#define ETABLE_LOGGER \
_E(1, TOOMANY, "number of unique categories exceeds the limit") \
_E(2, ILLREPEAT, "category repeated non-consecutively") \
_E(3, ILLORDER, "incompatible ordering of category definitions") \
_E(4, BADFILTER, "invalid filter expression") \
_E(5, NXSTREAM, "stream definition has unknown type") \

#if XAPI_STACKTRACE
#define LOGGER_TOOMANY perrtab_LOGGER->v[1 - ERRMIN_LOGGER].exit
#define LOGGER_ILLREPEAT perrtab_LOGGER->v[2 - ERRMIN_LOGGER].exit
#define LOGGER_ILLORDER perrtab_LOGGER->v[3 - ERRMIN_LOGGER].exit
#define LOGGER_BADFILTER perrtab_LOGGER->v[4 - ERRMIN_LOGGER].exit
#define LOGGER_NXSTREAM perrtab_LOGGER->v[5 - ERRMIN_LOGGER].exit
#else
enum {
#define _E(a, b, c) LOGGER_ ## b = a,
ETABLE_LOGGER
#undef _E
};
#endif
#define ERRMIN_LOGGER 1
#define ERRMAX_LOGGER 5
extern errtab * perrtab_LOGGER;
#endif
