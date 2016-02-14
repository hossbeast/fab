#include "NARRATE.errtab.h"
#define APIDATA
etable errtab_NARRATE = {
   .name = "NARRATE"
 , .min = ERRMIN_NARRATE
 , .max = ERRMAX_NARRATE
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_NARRATE * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_NARRATE
#undef _E
}};
etable * APIDATA perrtab_NARRATE = &errtab_NARRATE;
