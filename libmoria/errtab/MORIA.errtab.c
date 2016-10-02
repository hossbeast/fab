#include "MORIA.errtab.h"
#define APIDATA
errtab errtab_MORIA = {
   .name = "MORIA"
 , .min = ERRMIN_MORIA
 , .max = ERRMAX_MORIA
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_MORIA ] = { .name = #b, .desc = c, .str ="MORIA_" #b " : " c, .exit = a },
ETABLE_MORIA
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_MORIA = &errtab_MORIA;
#endif
