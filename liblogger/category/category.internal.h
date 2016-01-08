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

#ifndef _LOGGER_CATEGORY_INTERNAL_H
#define _LOGGER_CATEGORY_INTERNAL_H

#include <stdint.h>

#include "xapi.h"

#include "category.h"

/// category_max_name_length
//
// SUMMARY
//  length of the longest category name
//  written in logger_category_resolve
//
extern int category_max_name_length;

/// category_setup
//
// SUMMARY
//
//
xapi category_setup();

/// category_teardown
//
// SUMMARY
//  
//
void category_teardown();

/// category_attr_say
//
// SUMMARY
//
// PARAMETERS
//  attr     - 
//  narrator - 
//
xapi category_attr_say(uint32_t attr, narrator * _narrator)
  __attribute__((nonnull));

#endif
