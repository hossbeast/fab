#ifndef _FABC_ERRTAB_H
#define _FABC_ERRTAB_H
#include "xapi.h"
#define ETABLE_FABC \
_E(1, BADARGS, "failed to parse command-line arguments") \

enum {
#define _E(a, b, c) FABC_ ## b = a,
ETABLE_FABC
#undef _E
};
#define ERRMIN_FABC 1
#define ERRMAX_FABC 1
extern etable * perrtab_FABC;
#endif
