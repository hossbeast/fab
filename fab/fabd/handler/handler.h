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

#ifndef _HANDLER_H
#define _HANDLER_H

#include <sys/types.h>
#include <stdint.h>

struct map;         // map.h
struct gn;          // gn.h
struct ff_parser;   // ff.h
struct ts;          // ts.h

#define restrict __restrict

typedef struct handler_context
{
  struct pstring *      tmp;              // reusable temp space

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
} handler_context;

/// handler
//
// SUMMARY
//  handles a request
//
// PARAMETERS
//  ctx   - handler context instance
//  vmap  - init-level var map
//  first - first dependency from fabfiles
//
int handler(handler_context * const restrict ctx, struct map * const restrict vmap, struct gn * const restrict first)
  __attribute__((nonnull));

/// handler_context_mk
//
// SUMMARY
//
// PARAMETERS
//  ctx - (returns) handler context instance
//
int handler_context_mk(handler_context ** const restrict ctx)
  __attribute__((nonnull));

/// handler_context_reset
//
// SUMMARY
//  reset an handler_context instance
//
void handler_context_reset(handler_context * const restrict ctx)
  __attribute__((nonnull));

/// handler_context_free
//
// SUMMARY
//  free an handler_context instance with free semantics
//
void handler_context_free(handler_context * ctx);

/// handler_context_xfree
//
// SUMMARY
//  free an handler_context instance with xfree semantics
//
void handler_context_xfree(handler_context ** const restrict ctx)
  __attribute__((nonnull));

#endif
