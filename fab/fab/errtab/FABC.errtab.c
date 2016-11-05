#include "FABC.errtab.h"
#define APIDATA
etable errtab_FABC = {
   .name = "FABC"
 , .min = ERRMIN_FABC
 , .max = ERRMAX_FABC
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_FABC * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_FABC
#undef _E
}};
etable *  perrtab_FABC = &errtab_FABC;
