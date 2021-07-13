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

#ifndef _XPTHREAD_H
#define _XPTHREAD_H

#include <pthread.h>
#include <signal.h>

/// xpthread_attr_init
//
// SUMMARY
//  void proxy for pthread_attr_init
//
void xpthread_attr_init(pthread_attr_t * attr)
  __attribute__((nonnull));

/// xpthread_attr_destroy
//
// SUMMARY
//  void proxy for pthread_attr_destroy
//
void xpthread_attr_destroy(pthread_attr_t * attr)
  __attribute__((nonnull));

/// xpthread_create
//
// SUMMARY
//  void proxy for pthread_create
//
void xpthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void *), void * arg)
  __attribute__((nonnull(1, 3)));

/// xpthread_sigmask
//
// SUMMARY
//  void proxy for pthread_sigmask
//
void xpthread_sigmask(int how, const sigset_t * set, sigset_t * oldset);

/// xpthread_attr_setdetachstate
//
// SUMMARY
//  void proxy for pthread_attr_setdetachstate
//
void xpthread_attr_setdetachstate(pthread_attr_t * attr, int detachstate)
  __attribute__((nonnull));

#endif
