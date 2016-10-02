#include "XUNIT.errtab.h"
#define APIDATA
errtab errtab_XUNIT = {
   .name = "XUNIT"
 , .min = ERRMIN_XUNIT
 , .max = ERRMAX_XUNIT
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_XUNIT ] = { .name = #b, .desc = c, .str ="XUNIT_" #b " : " c, .exit = a },
ETABLE_XUNIT
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_XUNIT = &errtab_XUNIT;
#endif
