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

#ifndef _NARRATE_SAY_H
#define _NARRATE_SAY_H

#include <stdarg.h>
#include <sys/types.h>

#include "xapi.h"

struct narrator;					// narrator.h

#define restrict __restrict

/// narrate_vsayf
//
// SUMMARY
//  formatted write from va_list to the specified narrator
//
xapi narrate_vsayf(struct narrator * const restrict n, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

/// narrate_sayf
//
// SUMMARY
//  formatted write to the specified narrator
//
xapi narrate_sayf(struct narrator * const restrict n, const char * const restrict fmt, ...)
	__attribute__((nonnull(1, 2)));

/// narrate_sayw
//
// SUMMARY
//  write to the specified narrator
//
xapi narrate_sayw(struct narrator * const restrict n, char * const restrict b, size_t l)
	__attribute__((nonnull));

/// narrate_sayc
//
// SUMMARY
//  write a byte to the specified narrator
//
xapi narrate_sayc(struct narrator * const restrict n, int c)
	__attribute__((nonnull));

#undef restrict
#endif
