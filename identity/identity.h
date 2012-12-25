#ifndef _IDENTITY_H
#define _IDENTITY_H

#include "args.h"

/// identity_init
//
// populates the identity fields of g_args and assert that this
// executable was run with the correct user:group ownership and permissions
//
int identity_init();

/// identity_assume_user
//
// assume ruid identity, i.e. the user who executed the program
//
int identity_assume_user();

/// identity_assume_fabsys
//
// assume euid identity, i.e. fabsys:fabsys
//
int identity_assume_fabsys();

#endif
