#ifndef _LISTWISE_ERRTAB_H
#define _LISTWISE_ERRTAB_H
#include "xapi.h"
#define ETABLE_LISTWISE \
_E(1, BADFILE, "unable to process file") \
_E(2, BADARGS, "invalid command-line arguments") \

enum {
#define _E(a, b, c) LISTWISE_ ## b = a,
ETABLE_LISTWISE
#undef _E
};
#define ERRMIN_LISTWISE 1
#define ERRMAX_LISTWISE 2
extern etable * perrtab_LISTWISE;
#endif
