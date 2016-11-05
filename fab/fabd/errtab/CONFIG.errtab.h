#ifndef _CONFIG_ERRTAB_H
#define _CONFIG_ERRTAB_H
#include "xapi.h"
#define ETABLE_CONFIG \
_E(1, SYNTAX, "config could not be parsed") \
_E(2, ILLBYTE, "config contains illegal byte(s)") \
_E(3, NOTMAP, "top-level config value is not a map") \
_E(4, ILLEGAL, "config value missing but required or of an incompatible type") \
_E(5, INVALID, "config value outside the acceptable range") \

#if XAPI_STACKTRACE
#define CONFIG_SYNTAX perrtab_CONFIG->v[1 - ERRMIN_CONFIG].exit
#define CONFIG_ILLBYTE perrtab_CONFIG->v[2 - ERRMIN_CONFIG].exit
#define CONFIG_NOTMAP perrtab_CONFIG->v[3 - ERRMIN_CONFIG].exit
#define CONFIG_ILLEGAL perrtab_CONFIG->v[4 - ERRMIN_CONFIG].exit
#define CONFIG_INVALID perrtab_CONFIG->v[5 - ERRMIN_CONFIG].exit
#else
enum {
#define _E(a, b, c) CONFIG_ ## b = a,
ETABLE_CONFIG
#undef _E
};
#endif
#define ERRMIN_CONFIG 1
#define ERRMAX_CONFIG 5
extern errtab * perrtab_CONFIG;
#endif
