#include "LOGGER.errtab.h"
#define APIDATA
etable errtab_LOGGER = {
   .name = "LOGGER"
 , .min = ERRMIN_LOGGER
 , .max = ERRMAX_LOGGER
 , .v =  (typeof(((etable*)0)->v[0])[]) {
#define _E(a, b, c) [ a + (ERRMIN_LOGGER * -1) ] = { .name = #b, .desc = c, .str = #b " : " c },
ETABLE_LOGGER
#undef _E
}};
etable * APIDATA perrtab_LOGGER = &errtab_LOGGER;
