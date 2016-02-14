#ifndef _NARRATE_ERRTAB_H
#define _NARRATE_ERRTAB_H
#include "xapi.h"
#define ETABLE_NARRATE \
_E(1, CANTMARK, "marks are not supported for the narration type") \

enum {
#define _E(a, b, c) NARRATE_ ## b = a,
ETABLE_NARRATE
#undef _E
};
#define ERRMIN_NARRATE 1
#define ERRMAX_NARRATE 1
extern etable * perrtab_NARRATE;
#endif
