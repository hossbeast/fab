/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#ifndef FABD_PATTERN_SEARCH_H
#define FABD_PATTERN_SEARCH_H


struct pattern;
struct fsent;
struct set;
struct module;
struct llist;

/*
 * find nodes matching a pattern
 *
 * pattern    - match pattern
 * module     - module to begin matching at
 * modules    - all modules
 * [variants] - variants
 * matches    - (returns) matching nodes
 */
void pattern_search(
    /* 1 */ const struct pattern * restrict pattern
  , /* 2 */ const struct module * restrict module
  , /* 3 */ const struct llist * restrict modules
  , /* 4 */ const struct set * restrict variants
  , /* 5 */ struct set * restrict matches
  , /* 6 */ void (*dirnode_visit)(void * ctx, struct fsent * dirnode)
  , /* 7 */ void *dirnode_visit_ctx
)
  __attribute__((nonnull(1, 2, 3)));

void pattern_search_matches_create(struct set ** matches)
  __attribute__((nonnull));

#endif
