#include "KERNEL.errtab.h"
#define APIDATA
etable errtab_KERNEL = {
   .name = "KERNEL"
 , .min = ERRMIN_KERNEL
 , .max = ERRMAX_KERNEL
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_KERNEL * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_KERNEL
#undef _E
}};
#if XAPI_MODE_STACKTRACE
etable * APIDATA perrtab_KERNEL = &errtab_KERNEL;
#endif
