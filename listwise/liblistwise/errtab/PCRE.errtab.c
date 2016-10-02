#include "PCRE.errtab.h"
#define APIDATA
errtab errtab_PCRE = {
   .name = "PCRE"
 , .min = ERRMIN_PCRE
 , .max = ERRMAX_PCRE
 , .v =  (typeof(((errtab*)0)->v[0])[]) {
#define _E(a, b, c) [ a - ERRMIN_PCRE ] = { .name = #b, .desc = c, .str ="PCRE_" #b " : " c, .exit = a },
ETABLE_PCRE
#undef _E
}};
#if XAPI_STACKTRACE
errtab * APIDATA perrtab_PCRE = &errtab_PCRE;
#endif
