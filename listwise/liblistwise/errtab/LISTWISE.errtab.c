#include "LISTWISE.errtab.h"
#define APIDATA
errtab errtab_LISTWISE = {
   .name = "LISTWISE"
 , .min = ERRMIN_LISTWISE
 , .max = ERRMAX_LISTWISE
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_LISTWISE ] = { .name = #b, .desc = c, .str ="LISTWISE_" #b " : " c, .exit = a },
ETABLE_LISTWISE
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_LISTWISE = &errtab_LISTWISE;
#endif
