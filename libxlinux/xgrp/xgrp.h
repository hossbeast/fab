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

#ifndef _XGRP_H
#define _XGRP_H

#include <sys/types.h>
#include <grp.h>

#include "xapi.h"

#define restrict __restrict

/// uxgetgrgid_r
//
// SUMMARY
//  proxy for getgrgid_r that only fails when errno != { ENOENT, ESRCH, EBAD, EPERM }
//
xapi uxgetgrgid_r(gid_t uid, struct group * grp, char * buf, size_t buflen, struct group ** result);

/// xgetgrgid
//
// SUMMARY
//  proxy for getgrgid
//
xapi xgetgrgid(gid_t gid, struct group ** const restrict grp)
	__attribute__((nonnull));

#undef restrict
#endif
