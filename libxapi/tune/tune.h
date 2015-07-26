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

#ifndef _XAPI_TUNE_H
#define _XAPI_TUNE_H

// tune parameters - see tune.internal.h for defaults

/// xapi_dangle_factor
//
// SUMMARY
//  number of available frames at which reallocation takes place
//
// DEFAULT
//  
//
extern float xapi_dangle_factor;

/// xapi_stacks_factor
//
// SUMMARY
//  minimum extra stacks allocation as a multiple of the frame depth
//
// REMARKS
//  2 + (frame depth * stacks_factor)
//
// DEFAULT
//  .2f
//
extern float xapi_stacks_factor;

/// xapi_infos_factor
//
// SUMMARY
//  minimum infos allocation as a multiple of the stacks allocation
//
// DEFAULT
//  5
//
extern float xapi_infos_factor;

/// xapi_strings_factor
//
// SUMMARY
//  minimum allocation for each string
//
// DEFAULT
//  128
//
extern float xapi_strings_factor;

#endif
