#ifndef _XAPI_ERRTAB_H
#define _XAPI_ERRTAB_H
#include "xapi.h"
#define ETABLE_XAPI \
_E(1, ILLFATAL, "non-xapi function invoked with fatal") \
_E(2, NOFATAL, "xapi function invoked without fatal") \
_E(3, NOCODE, "fail invoked without a code") \
_E(4, NOTABLE, "fail invoked without a table") \
_E(5, NOMEM, "out of memory (not calltree related)") \
_E(6, AUNLOAD, "asymmetric load-unload sequence") \

enum {
#define _E(a, b, c) XAPI_ ## b = a,
ETABLE_XAPI
#undef _E
};
#define ERRMIN_XAPI 1
#define ERRMAX_XAPI 6
extern etable * perrtab_XAPI;
#endif
