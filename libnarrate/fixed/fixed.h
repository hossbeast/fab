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

#ifndef _NARRATE_FIXED_H
#define _NARRATE_FIXED_H

#define fvsayf(fmt, lst) narrate_fvsayf(_narrator, fmt, lst)
#define fsayf(fmt, ...)  narrate_fsayf (_narrator, fmt, ##__VA_ARGS__) 
#define fsays(s)         narrate_fsayw (_narrator, s)
#define fsayw(b, l)      narrate_fsayw (_narrator, b, l)               
#define fsayc(s, c)      narrate_fsayc (_narrator, c)                  

struct narrator;          // narrator.h

/// narrate_vsay
//
// SUMMARY
//  fixed-narration-specific say interface that returns void and is therefore
//  suitable for use within a finally block
//
// REMARKS
//  see say.h
//

#define restrict __restrict

void narrate_fvsayf(struct narrator * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

void narrate_fsayf(struct narrator * const restrict n, const char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2)));

void narrate_fsayw(struct narrator * const restrict n, char * const restrict b, size_t l)
  __attribute__((nonnull));

void narrate_fsayc(struct narrator * const restrict n, int c)
  __attribute__((nonnull));

#undef restrict
#endif
