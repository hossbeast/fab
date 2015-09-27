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

#ifndef _XLINUX_H
#define _XLINUX_H

#include "xapi.h"

/// xlinux_setup
//
// SUMMARY
//  initialize the library (idempotent)
//
xapi xlinux_setup();

/// xlinux_teardown
//
// SUMMARY
//  free resources used by the library
//
void xlinux_teardown();

#include "xlinux/xdirent.h"
#include "xlinux/xdlfcn.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xfile.h"
#include "xlinux/xftw.h"
#include "xlinux/xgrp.h"
#include "xlinux/xmman.h"
#include "xlinux/xprctl.h"
#include "xlinux/xpwd.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstat.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xtime.h"
#include "xlinux/xunistd.h"
#include "xlinux/xuio.h"
#include "xlinux/xwait.h"
#include "xlinux/xstdio.h"

#endif
