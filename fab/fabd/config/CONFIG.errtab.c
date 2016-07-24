#include "CONFIG.errtab.h"
#define APIDATA
etable errtab_CONFIG = {
   .name = "CONFIG"
 , .min = ERRMIN_CONFIG
 , .max = ERRMAX_CONFIG
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_CONFIG * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_CONFIG
#undef _E
}};
etable * APIDATA perrtab_CONFIG = &errtab_CONFIG;
