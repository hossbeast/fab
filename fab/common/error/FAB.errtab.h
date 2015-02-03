#ifndef _FAB_ERRTAB_H
#define _FAB_ERRTAB_H
#include "xapi.h"
#define ETABLE_FAB \
_E(1, BADARGS, "invalid command-line arguments") \
_E(2, BADTMP, "unexpected file(s) in tmp directory") \
_E(3, BADIPC, "ipc failure") \
_E(4, SYNTAX, "fabfile could not be parsed") \
_E(5, ILLBYTE, "fabfile contains illegal byte(s)") \
_E(6, UNSATISFIED, "buildplan has unsatisfied dependencies") \
_E(7, FMLFAIL, "formula failed to execute successfully") \
_E(8, DSCPARSE, "dependency discovery results were not parsed") \
_E(9, NOINVOKE, "invocation target could not be resolved") \
_E(10, BADPLAN, "buildplan cannot be constructed") \
_E(11, CYCLE, "dependency graph contains a cycle") \
_E(12, NOSELECT, "selector matches nothing") \
_E(13, NXPARAMS, "unable to determine runtime parameters") \
_E(14, FABDFAIL, "fabd terminated abnormally") \
_E(15, FABPERMS, "fab binar(ies) do not have proper permissions") \

enum {
#define _E(a, b, c) FAB_ ## b = a,
ETABLE_FAB
#undef _E
};
#define ERRMIN_FAB 1
#define ERRMAX_FAB 15
extern etable * perrtab_FAB;
#endif
