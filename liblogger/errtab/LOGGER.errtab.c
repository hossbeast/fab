#include "LOGGER.errtab.h"
#define APIDATA
errtab errtab_LOGGER = {
   .name = "LOGGER"
 , .min = ERRMIN_LOGGER
 , .max = ERRMAX_LOGGER
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_LOGGER ] = { .name = #b, .desc = c, .str ="LOGGER_" #b " : " c, .exit = a },
ETABLE_LOGGER
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_LOGGER = &errtab_LOGGER;
#endif
