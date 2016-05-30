#include "FAB.errtab.h"
#define APIDATA
etable errtab_FAB = {
   .name = "FAB"
 , .min = ERRMIN_FAB
 , .max = ERRMAX_FAB
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_FAB * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_FAB
#undef _E
}};
etable * APIDATA perrtab_FAB = &errtab_FAB;
