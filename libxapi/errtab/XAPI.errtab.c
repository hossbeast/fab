#include "XAPI.errtab.h"
#define APIDATA
etable errtab_XAPI = {
   .name = "XAPI"
 , .min = ERRMIN_XAPI
 , .max = ERRMAX_XAPI
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_XAPI * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_XAPI
#undef _E
}};
etable * APIDATA perrtab_XAPI = &errtab_XAPI;
