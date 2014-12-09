#ifndef _FAB_ERRTAB_H
#define _FAB_ERRTAB_H
#include "xapi.h"
#define ETABLE_FAB \
_E(1, BADARGS, "invalid command-line arguments") \
_E(2, BADIPC, "unexpected file(s) in ipc directory") \
_E(3, SYNTAX, "fabfile could not be parsed") \
_E(4, ILLBYTE, "fabfile contains illegal byte(s)") \
_E(5, BADTMP, "unexpected file(s) in temp directory") \
_E(6, BADCACHE, "unexpected file(s) in cache directory") \
_E(7, UNSATISFIED, "buildplan has unsatisfied dependencies") \
_E(8, FMLFAIL, "formula failed to execute successfully") \
_E(9, DSCPARSE, "dependency discovery results were not parsed") \
_E(10, NOINVOKE, "invocation target could not be resolved") \
_E(11, BADPLAN, "buildplan cannot be constructed") \
_E(12, EXESUID, "fab executable does not have ug+s permissions") \
_E(13, CYCLE, "dependency graph contains a cycle") \
_E(14, NOSELECT, "selector matches nothing") \
_E(15, NXPARAMS, "unable to determine runtime parameters") \
_E(16, FABDCRED, "prior execution had different ruid/rgid; re-run with -B") \

enum {
#define _E(a, b, c) FAB_ ## b = a,
ETABLE_FAB
#undef _E
};
#define ERRMIN_FAB 1
#define ERRMAX_FAB 16
extern etable * perrtab_FAB;
#endif
