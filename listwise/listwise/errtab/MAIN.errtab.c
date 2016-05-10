#include "MAIN.errtab.h"
#define APIDATA
etable errtab_MAIN = {
   .name = "MAIN"
 , .min = ERRMIN_MAIN
 , .max = ERRMAX_MAIN
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_MAIN * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_MAIN
#undef _E
}};
etable *  perrtab_MAIN = &errtab_MAIN;
