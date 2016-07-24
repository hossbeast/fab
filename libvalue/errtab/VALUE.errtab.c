#include "VALUE.errtab.h"
#define APIDATA
etable errtab_VALUE = {
   .name = "VALUE"
 , .min = ERRMIN_VALUE
 , .max = ERRMAX_VALUE
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_VALUE * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_VALUE
#undef _E
}};
etable * APIDATA perrtab_VALUE = &errtab_VALUE;
