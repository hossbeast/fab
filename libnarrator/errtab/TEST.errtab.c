#include "TEST.errtab.h"
#define APIDATA
etable errtab_TEST = {
   .name = "TEST"
 , .min = ERRMIN_TEST
 , .max = ERRMAX_TEST
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_TEST * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_TEST
#undef _E
}};
etable * APIDATA perrtab_TEST = &errtab_TEST;
