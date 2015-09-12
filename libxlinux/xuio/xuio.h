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

#ifndef _XLINUX_XUIO_H
#define _XLINUX_XUIO_H

#include <sys/uio.h>

#include "xapi.h"

/// xreadv
//
// SUMMARY
//  xapi proxy for readv
//
xapi xreadv(int fd, const struct iovec * iov, int iovcnt)
	__attribute__((nonnull(2)));

/// axreadv
//
// SUMMARY
//  xapi proxy for readv that also fails when count != actual
//
xapi axreadv(int fd, const struct iovec * iov, int iovcnt)
	__attribute__((nonnull(2)));

/// xwritev
//
// SUMMARY
//  xapi proxy for writev
//
xapi xwritev(int fd, const struct iovec * iov, int iovcnt)
	__attribute__((nonnull(2)));

/// axwritev
//
// SUMMARY
//  xapi proxy for writev that also fails when actual != count
//
xapi axwritev(int fd, const struct iovec * iov, int iovcnt)
	__attribute__((nonnull(2)));

#endif
