/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
