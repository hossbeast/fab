/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#ifndef FABD_VAR_H
#define FABD_VAR_H

/*

SUMMARY

*/

#include "types.h"
#include "xapi.h"

struct config;
struct node;
struct value;
struct variant;
struct value_parser;

/* global vars from config */
struct value * g_var;

/*
 * load a vars file
 *
 * @param var_node  - node for the var.bam file
 * @param var_value - (returns) parsed bacon
 */
xapi var_node_parse(struct node * restrict var_node)
  __attribute__((nonnull));

/*
 * out - (returns) key -> value set
 */
xapi var_denormalize(struct value_parser * restrict parser, struct variant * restrict var, struct value * restrict valset, struct value ** restrict out)
  __attribute__((nonnull(1, 3, 4)));

xapi var_setup(void);
xapi var_cleanup(void);

/// var_reconfigure
//
// SUMMARY
//  apply configuration changes
//
xapi var_reconfigure(struct config * restrict cfg, bool dry)
  __attribute__((nonnull));

#endif
