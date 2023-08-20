/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"


/// xstrncat
//
// reallocates *s1, appends s2 onto *s1
//
void ixstrncat(char ** s1, const char * s2, int s2len);

/// xstrcat
//
// reallocates *s1, appends s2 onto *s1
//
void ixstrcat(char ** s1, const char * s2);

/// xstrcatf
//
// calculates size required to vprintf, reallocates *s and appends the new string onto *s
//
void ixstrcatf(char ** s, char * fmt, ...);

/// xstrdup
//
// copies s2 to *s1, reallocating *s1 if necessary, deallocating it if it was already allocated
//
void ixstrdup(char ** s1, const char * s2);

/// xstrndup
//
// copies s2 to *s1, reallocating *s1 if necessary, deallocating it if it was already allocated
//
void ixstrndup(char ** s1, const char * s2, const size_t l);

/// ixsprintf
//
// - free(*s)
// - allocate(*s)
// - sprintf(fmt, ...) -> *s
//
void ixsprintf(char ** s, char * fmt, ...);
