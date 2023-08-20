/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <dlfcn.h>


/// xdlopen
//
// SUMMARY
//  fatalize-able wrapper for dlopen
//
void * xdlopen(const char * restrict filename, int flag)
  __attribute__((nonnull));

/// xdlclose
//
// SUMMARY
//  fatalize-able wrapper for dlclose
//
void xdlclose(void * dl)
  __attribute__((nonnull));

/// ixdlclose
//
// SUMMARY
//  idempotent xapi-proxy for dlclose that sets *dl = 0
//
void ixdlclose(void ** dl)
  __attribute__((nonnull));

/// xdlsym
//
// SUMMARY
//  fatalize-able wrapper for dlsym
//
void *xdlsym(void * restrict dl, const char * restrict sym)
  __attribute__((nonnull));

/// uxdlsym
//
// SUMMARY
//  xapi-proxy for dlsym that does not fail when the symbol is not found
//
void *uxdlsym(void * restrict dl, const char * restrict sym)
  __attribute__((nonnull));
