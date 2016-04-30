#include "LW.errtab.h"
#define APIDATA
etable errtab_LW = {
   .name = "LW"
 , .min = ERRMIN_LW
 , .max = ERRMAX_LW
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_LW * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_LW
#undef _E
}};
etable * APIDATA perrtab_LW = &errtab_LW;
