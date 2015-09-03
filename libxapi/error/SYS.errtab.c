#include "SYS.errtab.h"
#define APIDATA
etable errtab_SYS = {
   .tag = "SYS"
 , .min = ERRMIN_SYS
 , .max = ERRMAX_SYS
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_SYS * -1) ] = { .name = #b, .desc = c },
ETABLE_SYS
#undef _E
}};
etable * APIDATA perrtab_SYS = &errtab_SYS;
