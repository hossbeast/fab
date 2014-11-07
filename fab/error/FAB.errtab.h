#ifndef _FAB_ERRTAB_H
#define _FAB_ERRTAB_H
#include "xapi.h"
#define ETABLE_FAB \
_E(1, SYNTAX, "fabfile could not be parsed") \
_E(2, ILLBYTE, "fabfile contains illegal byte(s)") \
_E(3, BADTMP, "unexpected file(s) in temp directory") \
_E(4, BADCACHE, "unexpected file(s) in cache directory") \
_E(5, BADARGS, "invalid command-line arguments") \
_E(6, UNSATISFIED, "buildplan has unsatisfied dependencies") \
_E(7, FMLFAIL, "formula failed to execute successfully") \
_E(8, DSCPARSE, "dependency discovery results were not parsed") \
_E(9, NOINVOKE, "invocation target could not be resolved") \
_E(10, BADPLAN, "buildplan cannot be constructed") \
_E(11, EXESUID, "fab executable does not have ug+s permissions") \
_E(12, CYCLE, "dependency graph contains a cycle") \
_E(13, NOSELECT, "selector matches nothing") \
_E(14, NXPARAMS, "unable to determine runtime parameters") \

enum {
#define _E(a, b, c) FAB_ ## b = a,
ETABLE_FAB
#undef _E
};
#define ERRMIN_FAB 1
#define ERRMAX_FAB 14
extern etable * perrtab_FAB;
#endif
