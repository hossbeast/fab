#include "LISTWISE.errtab.h"
#define APIDATA
etable errtab_LISTWISE = {
   .name = "LISTWISE"
 , .min = ERRMIN_LISTWISE
 , .max = ERRMAX_LISTWISE
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_LISTWISE * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_LISTWISE
#undef _E
}};
etable * APIDATA perrtab_LISTWISE = &errtab_LISTWISE;
