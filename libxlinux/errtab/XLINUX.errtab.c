#include "XLINUX.errtab.h"
#define APIDATA
errtab errtab_XLINUX = {
   .name = "XLINUX"
 , .min = ERRMIN_XLINUX
 , .max = ERRMAX_XLINUX
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_XLINUX ] = { .name = #b, .desc = c, .str ="XLINUX_" #b " : " c, .exit = a },
ETABLE_XLINUX
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_XLINUX = &errtab_XLINUX;
#endif
