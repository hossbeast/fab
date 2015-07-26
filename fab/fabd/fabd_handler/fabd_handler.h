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

#ifndef _FABD_HANDLER_H
#define _FABD_HANDLER_H

#include <sys/types.h>
#include <stdint.h>

#include "executor.def.h"

struct map;               // map.h
struct gn;                // gn.h
struct ff_parser;         // ff.h
struct ts;                // ts.h

#define restrict __restrict

typedef struct fabd_handler_context
{
  executor_context;

  int stage;    // stage index

  struct gn **          fabrications;     // node lists
  size_t                fabricationsl;
  struct gn **          fabricationxs;
  size_t                fabricationxsl;
  struct gn **          fabricationns;
  size_t                fabricationnsl;
  struct gn **          invalidations;
  size_t                invalidationsl;
  struct gn **          discoveries;
  size_t                discoveriesl;
  struct gn **          inspections;
  size_t                inspectionsl;
  struct gn **          queries;
  size_t                queriesl;

  struct bp *           plan;             // buildplan
  struct map *          smap;             // selector map (must have lifetime >= lifetime of the selector lists)
  struct lwx **         stax;             // listwise stacks
  size_t                staxa;
  size_t                staxp;
  struct ts **          tsp;              // threadspaces
  size_t                tsa;
  size_t                tsl;
  size_t                tsw;
  struct map *          bakemap;
  struct ff_parser *    ffp;              // fabfile parser
} fabd_handler_context;

/// fabd_handler_context_mk
//
// SUMMARY
//
// PARAMETERS
//  ctx - (returns) fabd_handler context instance
//
int fabd_handler_context_mk(fabd_handler_context ** const restrict ctx)
  __attribute__((nonnull));

/// fabd_handler_context_reset
//
// SUMMARY
//  reset an fabd_handler_context instance
//
void fabd_handler_context_reset(fabd_handler_context * const restrict ctx)
  __attribute__((nonnull));

/// fabd_handler_context_free
//
// SUMMARY
//  free an fabd_handler_context instance with free semantics
//
void fabd_handler_context_free(fabd_handler_context * ctx);

/// fabd_handler_context_xfree
//
// SUMMARY
//  free an fabd_handler_context instance with xfree semantics
//
void fabd_handler_context_xfree(fabd_handler_context ** const restrict ctx)
  __attribute__((nonnull));

/// fabd_handler_handle_request
//
// SUMMARY
//  handles a request
//
// PARAMETERS
//  ctx   - fabd_handler context instance
//  vmap  - init-level var map
//  first - first dependency from fabfiles
//
int fabd_handler_handle_request(fabd_handler_context * const restrict ctx, struct map * const restrict vmap, struct gn * const restrict first)
  __attribute__((nonnull));

#endif
