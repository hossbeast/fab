#include "KERNEL.errtab.h"
#define APIDATA
errtab errtab_KERNEL = {
   .name = "KERNEL"
 , .min = ERRMIN_KERNEL
 , .max = ERRMAX_KERNEL
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_KERNEL ] = { .name = #b, .desc = c, .str ="KERNEL_" #b " : " c, .exit = a },
ETABLE_KERNEL
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_KERNEL = &errtab_KERNEL;
#endif
