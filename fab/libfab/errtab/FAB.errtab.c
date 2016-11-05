#include "FAB.errtab.h"
#define APIDATA
errtab errtab_FAB = {
   .name = "FAB"
 , .min = ERRMIN_FAB
 , .max = ERRMAX_FAB
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_FAB ] = { .name = #b, .desc = c, .str ="FAB_" #b " : " c, .exit = a },
ETABLE_FAB
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_FAB = &errtab_FAB;
#endif
