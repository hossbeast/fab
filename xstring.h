/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _XSTRING_H
#define _XSTRING_H

#define restrict __restrict

/* wrappers around certain functions from string.h */

/// xstrcmp
//
// semantically equivalent to strcmp, except that this function accepts length values
// for the two strings, which need not be null-terminated.
//
// not suitable for comparing numbers by passing pointers to them (although this would
// work on a big-endian machine)
//
// parameters
//
// A    - first string
// alen - length of A, or 0 if length of A == strlen(A)
// B    - second string
// blen - length of B, or 0 if length of B == strlen(B)
//
int xstrcmp(char* A, int alen, char* B, int blen, int case_insensitive);

/// xstrstr
//
// locates the byte sequence { B, B + blen } within the byte sequence { A, A + alen }
//
// if case_insensitive is set, those bytes in A and B in the range { 'A', 'z' } are compared
// without respect to case
//
// returns a pointer offset from A where B was found, or 0
//
char* xstrstr(char * restrict A, int alen, const char * const restrict B, int blen, const int case_insensitive)
	__attribute__((nonnull));

/// xstrcatf
//
// calculates size required to vprintf, reallocates *s and appends the new string onto *s
//
// returns zero on malloc failure
//
int xstrcatf(char** s, char* fmt, ...);

/// xstrcat
//
// reallocates *s1, appends s2 onto *s1
//
// returns zero on malloc failure
//
int xstrcat(char** s1, const char* s2);

/// xstrdup
//
// copies s2 to *s1, reallocating *s1 if necessary, deallocating it if it was already allocated
//
// returns zero on malloc failure
//
int xstrdup(char** s1, const char* s2);

/// xsprintf
//
// - free(*s)
// - allocate(*s)
// - sprintf(fmt, ...) -> *s
//
int xsprintf(char** s, char* fmt, ...);

#undef restrict
#endif

