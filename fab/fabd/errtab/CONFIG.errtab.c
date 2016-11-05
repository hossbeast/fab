#include "CONFIG.errtab.h"
#define APIDATA
errtab errtab_CONFIG = {
   .name = "CONFIG"
 , .min = ERRMIN_CONFIG
 , .max = ERRMAX_CONFIG
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_CONFIG ] = { .name = #b, .desc = c, .str ="CONFIG_" #b " : " c, .exit = a },
ETABLE_CONFIG
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_CONFIG = &errtab_CONFIG;
#endif
