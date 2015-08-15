#ifndef _XAPI_ERRTAB_H
#define _XAPI_ERRTAB_H
#include "xapi.h"
#define ETABLE_XAPI \
_E(1, ILLFATAL, "non xapi-enabled function invoked with fatal") \
_E(2, NOFATAL, "xapi-enabled function invoked without fatal") \
_E(3, NOMEM, "out of memory") \

enum {
#define _E(a, b, c) XAPI_ ## b = a,
ETABLE_XAPI
#undef _E
};
#define ERRMIN_XAPI 1
#define ERRMAX_XAPI 3
extern etable * perrtab_XAPI;
#endif
