#include "FABD.errtab.h"
#define APIDATA
errtab errtab_FABD = {
   .name = "FABD"
 , .min = ERRMIN_FABD
 , .max = ERRMAX_FABD
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_FABD ] = { .name = #b, .desc = c, .str ="FABD_" #b " : " c, .exit = a },
ETABLE_FABD
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_FABD = &errtab_FABD;
#endif
