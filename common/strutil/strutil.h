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

#ifndef _STRUTIL_H
#define _STRUTIL_H

#define restrict __restrict

/// estrcmp
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
int estrcmp(const char* A, int alen, const char* B, int blen, int case_insensitive);

/// estrstr
//
// locates the byte sequence { B, B + blen } within the byte sequence { A, A + alen }
//
// if case_insensitive is set, those bytes in A and B in the range { 'A', 'z' } are compared
// without respect to case
//
// returns a pointer offset from A where B was found, or 0
//
char* estrstr(char * restrict A, int alen, const char * const restrict B, int blen, const int case_insensitive)
  __attribute__((nonnull));

/// stresc
//
// SUMMARY
//  write a string to a buffer with all nonprinting characters and non-space whitespace replaced
//  with hex escape sequences
//
// PARAMETERS
//  src - source string
//  len - length of src, 0 for strlen
//  dst - dst buffer
//  sz  - size of dst
//
// RETURNS
//  returns number of bytes written
//
int stresc(char * const restrict src, const size_t len, char * const restrict dst, const size_t sz)
  __attribute__((nonnull(1, 3)));

#undef restrict
#endif
