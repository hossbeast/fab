#include "VALUE.errtab.h"
#define APIDATA
errtab errtab_VALUE = {
   .name = "VALUE"
 , .min = ERRMIN_VALUE
 , .max = ERRMAX_VALUE
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_VALUE ] = { .name = #b, .desc = c, .str ="VALUE_" #b " : " c, .exit = a },
ETABLE_VALUE
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_VALUE = &errtab_VALUE;
#endif
