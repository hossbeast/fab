#include "MAIN.errtab.h"
#define APIDATA
errtab errtab_MAIN = {
   .name = "MAIN"
 , .min = ERRMIN_MAIN
 , .max = ERRMAX_MAIN
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_MAIN ] = { .name = #b, .desc = c, .str ="MAIN_" #b " : " c, .exit = a },
ETABLE_MAIN
#undef _E
}};
#if XAPI_STACKTRACE
errtab *  perrtab_MAIN = &errtab_MAIN;
#endif
