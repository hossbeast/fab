#include "TEST.errtab.h"
#define APIDATA
errtab errtab_TEST = {
   .name = "TEST"
 , .min = ERRMIN_TEST
 , .max = ERRMAX_TEST
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_TEST ] = { .name = #b, .desc = c, .str ="TEST_" #b " : " c, .exit = a },
ETABLE_TEST
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_TEST = &errtab_TEST;
#endif
