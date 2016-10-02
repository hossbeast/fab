#include "MORIA.errtab.h"
#define APIDATA
etable errtab_MORIA = {
   .name = "MORIA"
 , .min = ERRMIN_MORIA
 , .max = ERRMAX_MORIA
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_MORIA * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_MORIA
#undef _E
}};
etable * APIDATA perrtab_MORIA = &errtab_MORIA;
