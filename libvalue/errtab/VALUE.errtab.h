#ifndef _VALUE_ERRTAB_H
#define _VALUE_ERRTAB_H
#include "xapi.h"
#define ETABLE_VALUE \
_E(1, DIFFTYPE, "cant merge values of differing type") \
_E(2, BADTYPE, "merge trees must have aggregate type") \

enum {
#define _E(a, b, c) VALUE_ ## b = a,
ETABLE_VALUE
#undef _E
};
#define ERRMIN_VALUE 1
#define ERRMAX_VALUE 2
extern etable * perrtab_VALUE;
#endif
