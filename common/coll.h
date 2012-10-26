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

#define coll_singly_add_onfail "c: 0x%08x, el: 0x%08x, ret: 0x%08x"
int coll_singly_add(coll_singly* c, void* el, void* ret);

void coll_singly_free(coll_singly* c);

#define coll_doubly_add_onfail "c: 0x%08x, el: 0x%08x, ret: 0x%08x"
int coll_doubly_add(coll_doubly* c, void* el, void* ret);

void coll_doubly_free(coll_doubly* c);

#endif
