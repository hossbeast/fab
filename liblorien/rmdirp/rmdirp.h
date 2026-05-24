/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/// rmdirp
//
// SUMMARY
//  recursively delete a directory and all of its contents, which consists only
//  of files, directories, symlinks, and dangling symlinks. symlinks are deleted
//  not followed
//
// PARAMETERS
//  dirpath -
//  rmself  - if true, delete directory itself as well
//  errcode - error code to raise if unexpected files are encountered
//
void rmdirp(const char * const restrict dirpath, int rmself)
  __attribute__((nonnull));
