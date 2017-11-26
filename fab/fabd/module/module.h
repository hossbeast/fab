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

struct array;
struct list;
struct map;
struct node;

typedef struct module
{
  struct node * base;       // directory node for the module

  struct list * artifacts;  // list<node>
  struct list * rules;      // list<ff_node_rule>
  struct list * required;   // list<vertex> of required references
} module;

// list of loaded modules
extern struct array * g_modules;

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

#endif
