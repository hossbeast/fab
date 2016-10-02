#include "SYS.errtab.h"
#define APIDATA
errtab errtab_SYS = {
   .name = "SYS"
 , .min = ERRMIN_SYS
 , .max = ERRMAX_SYS
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_SYS ] = { .name = #b, .desc = c, .str ="SYS_" #b " : " c, .exit = a },
ETABLE_SYS
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_SYS = &errtab_SYS;
#endif
