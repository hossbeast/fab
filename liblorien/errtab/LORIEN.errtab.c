#include "LORIEN.errtab.h"
#define APIDATA
etable errtab_LORIEN = {
   .name = "LORIEN"
 , .min = ERRMIN_LORIEN
 , .max = ERRMAX_LORIEN
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_LORIEN * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_LORIEN
#undef _E
}};
etable * APIDATA perrtab_LORIEN = &errtab_LORIEN;
