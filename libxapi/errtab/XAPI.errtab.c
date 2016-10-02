#include "XAPI.errtab.h"
#define APIDATA
errtab errtab_XAPI = {
   .name = "XAPI"
 , .min = ERRMIN_XAPI
 , .max = ERRMAX_XAPI
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_XAPI ] = { .name = #b, .desc = c, .str ="XAPI_" #b " : " c, .exit = a },
ETABLE_XAPI
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_XAPI = &errtab_XAPI;
#endif
