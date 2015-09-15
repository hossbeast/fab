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

#ifndef _TMP_H
#define _TMP_H

#define restrict __restrict

/// tmp_cleanup
//
// SUMMARY
//  delete directories in the tmp dir that have expired
//
// PARAMETERS
//  not_expired       - consider directories for these pids to be not expired
//  not_expiredl      - length of not_expired
//  consider_expired  - consider directories for these pids to be expired
//  consider_expiredl - length of consider_expired
//
int tmp_cleanup(
	  const pid_t * const restrict not_expired
	, size_t not_expiredl
	, const pid_t * const restrict consider_expired
	, size_t consider_expiredl
);

#undef restrict
#endif
