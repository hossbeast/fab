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

#ifndef FABD_MODULE_H
#define FABD_MODULE_H

/*

SUMMARY
 A module is the smallest component which fab can build. 

*/

#include "types.h"
#include "xapi.h"

struct dictionary;
struct list;
struct map;
struct node;

typedef struct module
{
  struct node * base;         // directory node for the module
  bool leaf;                  // whether there do not exist more specific modules under this path

  struct list * artifacts;    // list<node>
  struct list * rules;        // list<ff_node_rule> of rules in the module
  struct list * rules_lists;  // list<list> of rules applicable to the module

  struct list * used;         // list<module> of used modules
  struct list * required;     // list<module> of required modules
  struct map * require_scope; // map<string, node> of directory nodes of required modules by name
} module;

// list of loaded modules
extern struct dictionary * g_modules;

xapi module_setup(void);
xapi module_cleanup(void);
xapi module_report(void);

/// module_load_project
//
// SUMMARY
//  load the module in the project directory, and nested modules, recursively
//
// PARAMETERS
//  project_root - base node for the starting module
//  project_dir  - absolute path for the 
//
xapi module_load_project(struct node * restrict root, const char * restrict absdir)
  __attribute__((nonnull));

/// module_lookup
//
// SUMMARY
//  get the module for the specified path, if any
//
// PARAMETERS
//  path - normalized absolute path
//  mod  - (returns) owning module, if any
//
module * module_lookup(const char * const restrict path, size_t pathl)
  __attribute__((nonnull));

#endif
