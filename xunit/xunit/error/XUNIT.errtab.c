#include "XUNIT.errtab.h"
#define APIDATA
etable errtab_XUNIT = {
   .tag = "XUNIT"
 , .min = ERRMIN_XUNIT
 , .max = ERRMAX_XUNIT
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_XUNIT * -1) ] = { .name = #b, .desc = c },
ETABLE_XUNIT
#undef _E
}};
etable *  perrtab_XUNIT = &errtab_XUNIT;
