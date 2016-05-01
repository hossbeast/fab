#include "LIB.errtab.h"
#define APIDATA
etable errtab_LIB = {
   .name = "LIB"
 , .min = ERRMIN_LIB
 , .max = ERRMAX_LIB
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_LIB * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_LIB
#undef _E
}};
etable * APIDATA perrtab_LIB = &errtab_LIB;
