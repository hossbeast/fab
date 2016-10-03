#ifndef _XLINUX_ERRTAB_H
#define _XLINUX_ERRTAB_H
#include "xapi.h"
#define ETABLE_XLINUX \
_E(1, DLERROR, "dynamic loader error") \
_E(2, FTWERROR, "filesystem traversal error") \
_E(3, LESS, "less data available than was expected") \
_E(4, MPOLICY, "mempolicy limit exceeded") \
_E(5, NAMETOOLONG, "name too long for static space") \

#if XAPI_STACKTRACE
#define XLINUX_DLERROR perrtab_XLINUX->v[1 - ERRMIN_XLINUX].exit
#define XLINUX_FTWERROR perrtab_XLINUX->v[2 - ERRMIN_XLINUX].exit
#define XLINUX_LESS perrtab_XLINUX->v[3 - ERRMIN_XLINUX].exit
#define XLINUX_MPOLICY perrtab_XLINUX->v[4 - ERRMIN_XLINUX].exit
#define XLINUX_NAMETOOLONG perrtab_XLINUX->v[5 - ERRMIN_XLINUX].exit
#else
enum {
#define _E(a, b, c) XLINUX_ ## b = a,
ETABLE_XLINUX
#undef _E
};
#endif
#define ERRMIN_XLINUX 1
#define ERRMAX_XLINUX 5
extern errtab * perrtab_XLINUX;
#endif
