#ifndef _CONFIG_ERRTAB_H
#define _CONFIG_ERRTAB_H
#include "xapi.h"
#define ETABLE_CONFIG \
_E(1, SYNTAX, "config could not be parsed") \
_E(1, ILLBYTE, "config contains illegal byte(s)") \

enum {
#define _E(a, b, c) CONFIG_ ## b = a,
ETABLE_CONFIG
#undef _E
};
#define ERRMIN_CONFIG 1
#define ERRMAX_CONFIG 1
extern etable * perrtab_CONFIG;
#endif
