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

#ifndef _LISTWISE_OBJECT_H
#define _LISTWISE_OBJECT_H

#include <stdint.h>
#include <sys/types.h>

#include "listwise.h"

struct lwx;     // lwx.h

#define restrict __restrict

/* LISTWISE OBJECT API
**
**  typically liblistwise manipulates lists of strings. using this api it is possible
**  to also manipulate lists of objects of various types
**
**  first, use listwise_register_object to associate a unique type identifier with an
**  object definition, which is a list of function pointers for manipulating instances
**  of that object
**
**  use the lstack_* api's in listwise/object.h add instances of the object to an lstack
**
**  these operators use the object api
**    - rx (reflect)
**
**  object instances are manipulated by traditional operators that do not modify lstack
**  entries through the object's string_property. This specifies a property which is to
**  be accessed on instances of the object to get a string representation of the instance
**
*/

typedef struct listwise_object
{
  /// type
  //
  // unique type identifier
  //
  uint8_t type;

  /// string_property
  //
  // SUMMARY
  //  active propery for default string coercion on object entries of this type
  //  at listwise_register_object time, if this value is not null, it should be
  //  set to a malloc()'d string, not a string constant
  //
  // DEFAULT
  //  0
  //
  char* string_property;

  /// string
  //
  // SUMMARY
  //  get string/length pointers which liblistwise uses to treat objects of
  //  this type as a string
  //
  // PARAMETERS
  //  o      - pointer to the object
  //  [prop] - name of string property, or null for default
  //  s      - pointer to string returned here
  //  l      - length returned here
  //
  // RETURNS
  //  0 on error, 1 otherwise
  //
  int (*string)(void * o, char* prop, char ** s, int * l);

  /// reflect
  //
  // SUMMARY
  //  access a property on an object of this type. usually this means replace the
  //  instance of the object with 1 or more other objects and/or strings
  //
  // DETAILS
  //  *rl is set to the number of entities in the reflected property
  //  foreach entity in the reflected property:
  //   if that entity is a string
  //    - *r[x] is set to a pointer to the string
  //    - *rls[x] is set to the length of that string
  //    - *rtypes[x] is ignored
  //    - *rls[x] bytes are immediately copied from *r[x] into the lstack
  //   if that entity is an object
  //    - *r[x] is set to a pointer to the object
  //    - *rls[x] is ignored
  //    - *rtypes[x] is set to the typeid of the listwise_register'd object
  //    - sizeof(void*) bytes are immediately copied from *r[x] into the lstack
  //
  // PARAMETERS
  //  o        - pointer to the object
  //  property - property name
  //  r        - replacement objects/strings
  //  rtypes   - list of types of length rl
  //  rls      - list of lengths of length rl
  //  rl       - number of returned objects/strings
  //
  // *r, *rtypes, and *rls are free()'d by the caller (liblistwise) after invoking this method
  //
  // RETURNS
  //  0 on error, 1 otherwise
  //
  int (*reflect)(void * o, char* property, void *** r, uint8_t ** rtypes, int ** rls, int * rl);

  /// destroy
  //
  // SUMMARY
  //  invoked just prior to forgetting the pointer to an object of this type
  //
  void (*destroy)(void* o);
} listwise_object;

///
/// [[ OBJECT API ]]
///

/// listwise_register
//
// SUMMARY
//  associate the specified type and object definition
// 
// PARAMETERS
//  type - unique user-defined object identifier
//  def  - object definition
// 
// RETURNS
//  0 on error (memory, io)
//
xapi listwise_register_object(uint8_t type, listwise_object * def);

/// listwise_enumerate_objects
//
// get a list of registered object definitions
//
// *list is deallocated by the caller
//
xapi listwise_enumerate_objects(listwise_object *** list, size_t * list_len);

/// listwise_lookup_object
//
// lookup the object definition associated with the specified type
//
xapi listwise_lookup_object(uint8_t type, listwise_object ** obj);

///
/// [[ LSTACK API (for objects) ]]
///

/// lstack_obj_write
//
// SUMMARY
//  place an object instance at x:y
//
// PARAMETERS
//  lx    - lw context
//  x     - list
//  y     - row
//  o     - pointer object
//  type  - object type
//
xapi lstack_obj_write(struct lwx* const restrict lx, int x, int y, const void* const restrict o, uint8_t type)
  __attribute__((nonnull));

/// lstack_obj_alt_write
//
// SUMMARY
//  place an object instance at x:y
//
// PARAMETERS
//  lx    - lw context
//  x     - list
//  y     - row
//  o     - pointer object
//  type  - object type
//
// REMARKS
//  used in conjunction with lstack_displace
//
xapi lstack_obj_alt_write(struct lwx* const restrict lx, int x, int y, const void* const restrict o, uint8_t type)
  __attribute__((nonnull));

/// lstack_add
//
// SUMMARY
//  place an object instance at the next unused entry of list 0
//
// PARAMETERS
//  lx    - lw context
//  o     - pointer object
//  type  - object type
//
xapi lstack_obj_add(struct lwx* const restrict lx, const void* const restrict o, uint8_t type)
  __attribute__((nonnull));

/// lstack_getobject
//
// SUMMARY
//  get pointer/type for the object stored in the specified row
//
// PARAMETERS
//  lx - lw context
//  x  - list index
//  y  - row index
//  r  - string returned here
//  rt - type returned here
//
// RETURNS
//  0 on success
//
// REMARKS
//  getobject will return 0 in rt for a non-object entry. getobject does not use tmp space
//
xapi lstack_getobject(struct lwx* const restrict lx, int x, int y, char ** const restrict r, uint8_t * const restrict rt)
  __attribute__((nonnull));

#endif
