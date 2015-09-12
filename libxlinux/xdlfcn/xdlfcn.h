/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _XDLFCN_H
#define _XDLFCN_H

#include <dlfcn.h>

#include "xapi.h"

/// xdlopen
//
// SUMMARY
//  fatalize-able wrapper for dlopen
//
xapi xdlopen(const char * filename, int flag, void ** dl)
	__attribute__((nonnull));

/// ixdlclose
//
// SUMMARY
//  idempotent xapi-proxy for dlclose that sets *dl = 0
//
xapi ixdlclose(void ** dl)
	__attribute__((nonnull));

/// xdlsym
//
// SUMMARY
//  fatalize-able wrapper for dlsym
//
xapi xdlsym(void * dl, const char * sym, void ** psym);

/// uxdlsym
//
// SUMMARY
//  xapi-proxy for dlsym that does not fail when the symbol is not found
//
xapi uxdlsym(void * dl, const char * sym, void ** psym);

#endif
