/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#ifndef _COLL_H
#define _COLL_H

///
/// LIGHTWEIGHT collection, implemented either as a doubly-allocated list
/// or singly-allocated list,  suitable for use with idx.h.
///
/// USAGE - example declaration : 
///
/// >> file://clients.h :
/// extern union g_clients_t
/// {
/// 	coll_doubly c;
/// 
/// 	struct
/// 	{
/// 		int				len;
/// 		int				alloc;
/// 		int				size;
/// 		client**	e;
/// 
/// 		idx*		by_key;
/// 		idx*		by_ipsid;
/// 	};
/// } g_clients;
///
/// >> file://clients.c :
/// union g_clients_t g_clients = { { .size = sizeof(client) } };
///
/// MEMORY MANAGEMENT - example cleanup (for doubly-allocated list) :
///
/// int x;
/// for(x = 0; x < g_clients.len; x++)
/// {
///   // free client fields
///		free(g_clients.e[x]->string);
///
///		// free memory for this client
///		free(g_clients.e[x]);
/// }
///
/// // free collection memory
/// free(g_clients.e);
///

typedef struct
{
	int 	len;			// initialized elements in e
	int 	alloc;		// allocated size of e (in elements)
	int 	size;			// size of elements in e
	char*	e;				// singly-allocated list of collection elements
} coll_singly;

typedef struct
{
	int 	len;			// initialized elements in e
	int 	alloc;		// allocated size of e (in elements)
	int 	size;			// size of elements pointed to by entries in e
	char*	e;				// doubly-allocated list of collection elements
} coll_doubly;

int coll_singly_add(coll_singly* c, void* el, void* ret);

void coll_singly_free(coll_singly* c);

int coll_doubly_add(coll_doubly* c, void* el, void* ret);

void coll_doubly_free(coll_doubly* c);

#endif

