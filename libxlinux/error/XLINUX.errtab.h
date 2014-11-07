#ifndef _XLINUX_ERRTAB_H
#define _XLINUX_ERRTAB_H
#include "xapi.h"
#define ETABLE_XLINUX \
_E(1, DLERROR, "dynamic loader error") \
_E(2, FTWERROR, "filesystem traversal error") \
_E(3, LESS, "less data available than was expected") \

enum {
#define _E(a, b, c) XLINUX_ ## b = a,
ETABLE_XLINUX
#undef _E
};
#define ERRMIN_XLINUX 1
#define ERRMAX_XLINUX 3
extern etable * perrtab_XLINUX;
#endif
