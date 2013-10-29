/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LISTWISE_RE_H
#define _LISTWISE_RE_H

#include <stdint.h>
#include <stdio.h>

#include "listwise/operator.h"

#define restrict __restrict

/*
** LISTWISE RE API
*/

/// re_compile
//
// compile the re for an op arg representing a regex
//
// parameters
//  re  - regex op arg
//  mod - modifier string arg, if any
//
int re_compile(char* s, struct re* re, char* mod);

/// re_exec
//
// execute an regex against a string
//
// parameters
//  re       - regex already compiled with re_compile
//  s        - subject string
//  l        - length of s
//  o        - offset from s to start matching
//  ovec     - results and subcapture information written to *ovec
//  ovec_len - *ovec size is at *ovec_len
//
// returns 1 on success, 0 on failure (memory allocation, bad pcre, etc)
//
// example
//    char* s;
//    int l;
//    int* ovec;
//    int ovec_len;
//		if(re_exec(re, s, l, 0, &ovec, &ovec_len) == 0)
//		{
//			/* failure */
//		}
//		else if(ovec[0] > 0)
//		{
//			int x;
//			for(x = 0; x < ovec[0]; x++)
//			{
//				int a = ovec[1 + (x * 2) + 0];
//				int b = ovec[1 + (x * 2) + 1];
//
//				if(a >= 0 && b >= 0)
//				{
//					/* print x'th subcapture - recall the 0th subcapture is the entire match */
//					printf("%.*s", b - a, s + a);
//				}
//			}
//		}
//		else
//		{
//			/* no match */
//		}
//
int re_exec(struct re* re, char* s, int l, int o, int** ovec, int* ovec_len);

#undef restrict
#endif
