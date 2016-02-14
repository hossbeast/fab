#include "XLINUX.errtab.h"
#define APIDATA
etable errtab_XLINUX = {
   .name = "XLINUX"
 , .min = ERRMIN_XLINUX
 , .max = ERRMAX_XLINUX
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_XLINUX * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_XLINUX
#undef _E
}};
etable * APIDATA perrtab_XLINUX = &errtab_XLINUX;
