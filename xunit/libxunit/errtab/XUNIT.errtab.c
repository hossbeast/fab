#include "XUNIT.errtab.h"
#define APIDATA
etable errtab_XUNIT = {
   .name = "XUNIT"
 , .min = ERRMIN_XUNIT
 , .max = ERRMAX_XUNIT
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_XUNIT * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_XUNIT
#undef _E
}};
etable * APIDATA perrtab_XUNIT = &errtab_XUNIT;