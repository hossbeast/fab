/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "common/assure.h"

#include "moria.h"
#include "edge.h"
#include "dispatch.h"
#include "operations.h"

static moria_vertex *identifier_lookup(moria_parser * restrict parser, llist * restrict first, llist * restrict end)
{
  moria_vertex *v;
  moria_vertex *p;
  moria_vertex *rv;
  struct label *label;
  llist *lln;
  int x;

  rv = 0;
  for(x = 0; x < parser->vertex_lists_len; x++) {
    llist_foreach(parser->vertex_lists[x], v, owner) {
      p = v;
      for(lln = first; lln != end; lln = lln->prev) {
        label = containerof(lln, typeof(*label), lln);
        if(!p) {
          break;
        }
        if(memncmp(p->label, p->label_len, label->s, label->len) != 0) {
          break;
        }

        /* upwards to the parent */
        if(p->up_identity) {
          p = p->up_identity->A;
        }
      }

      if(lln != end) {
        continue; // not all labels in the sequence matched
      }

      if(rv) {
        RTA;  // vertex names not unique
      }
      rv = v;
    }
  }

  return rv;
}

static moria_vertex *labels_resolve(moria_parser * restrict parser, llist * restrict first, llist * restrict end)
{
  moria_vertex *v;
  moria_vertex *pv;
  struct label *label;

  if((v = identifier_lookup(parser, first, end))) {
    return v;
  }

  /* create from the first label only */
  label = containerof(first, typeof(*label), lln);
  parser->dispatch->create_vertex(parser, &v, label->attrs, label->opattrs, label->s, label->len);

  if(first->prev == end) {
    return v;
  }
  pv = labels_resolve(parser, first->prev, end);
  parser->dispatch->connect(parser, v, pv, MORIA_EDGE_IDENTITY, 0);

  return v;
}

static moria_vertex *identifier_resolve(moria_parser * restrict parser, identifier * id)
{
  return labels_resolve(parser, id->labels.prev, &id->labels);
}

static void identifier_list_resolve(
    moria_parser * restrict parser
  , identifier_list * restrict ids
  , moria_vertex *** restrict list
  , size_t * restrict list_sz
  , size_t * restrict list_len
)
{
  struct identifier *identifier;
  moria_vertex *v;

  *list_len = 0;
  llist_foreach(&ids->identifiers, identifier, lln) {
    v = identifier_resolve(parser, identifier);
    assure(list, sizeof(**list), (*list_len) + 1, list_sz);
    (*list)[(*list_len)++] = v;
  }
}

//
// public
//

label *label_alloc(const char *s, uint16_t len)
{
  struct label *label;

  xmalloc(&label, sizeof(*label));
  label->s = s;
  label->len = len;
  llist_init(&label->lln);

  return label;
}

void label_free(struct label * restrict label)
{
  free(label);
}

identifier *identifier_alloc(struct label * restrict label, uint8_t opattrs)
{
  identifier *id;

  xmalloc(&id, sizeof(*id));
  label->opattrs = opattrs;
  llist_init(&id->lln);

  llist_init(&id->labels);
  llist_push(&id->labels, &label->lln);

  return id;
}

void identifier_free(struct identifier * identifier)
{
  struct label *label;

  while((label = llist_shift(&identifier->labels, typeof(*label), lln))) {
    label_free(label);
  }

  free(identifier);
}

identifier_list *identifier_list_alloc()
{
  identifier_list *ids;

  xmalloc(&ids, sizeof(*ids));
  llist_init(&ids->identifiers);

  return ids;
}

void identifier_list_free(identifier_list *ids)
{
  struct identifier *identifier;

  while((identifier = llist_shift(&ids->identifiers, typeof(*identifier), lln))) {
    identifier_free(identifier);
  }

  free(ids);
}

//
// operations
//

void operations_declare(moria_parser * restrict parser, identifier_list *list)
{
  struct identifier *identifier;

  llist_foreach(&list->identifiers, identifier, lln) {
    identifier_resolve(parser, identifier);
  }

  identifier_list_free(list);
}

void operations_refresh(moria_parser * restrict parser, identifier_list * restrict list)
{
  struct identifier *identifier;
  moria_vertex *v;

  llist_foreach(&list->identifiers, identifier, lln) {
    v = identifier_resolve(parser, identifier);
    parser->dispatch->refresh_vertex(parser, v);
  }

  identifier_list_free(list);
}

void operations_invalidate(moria_parser * restrict parser, identifier_list * restrict list)
{
  struct identifier *identifier;
  moria_vertex *v;

  llist_foreach(&list->identifiers, identifier, lln) {
    v = identifier_resolve(parser, identifier);
    parser->dispatch->invalidate_vertex(parser, v);
  }

  identifier_list_free(list);
}

void operations_connect(
    moria_parser * restrict parser
  , identifier_list * restrict B
  , identifier_list * restrict A
  , uint32_t attrs
)
{
  moria_vertex **Blist;
  size_t Blen;
  size_t Bsz;
  moria_vertex **Alist;
  size_t Alen;
  size_t Asz;

  Blist = 0;
  Blen = 0;
  Bsz = 0;
  identifier_list_resolve(parser, B, &Blist, &Bsz, &Blen);

  Alist = 0;
  Alen = 0;
  Asz = 0;
  identifier_list_resolve(parser, A, &Alist, &Asz, &Alen);

  parser->dispatch->hyperconnect(parser, Blist, Blen, Alist, Alen, attrs | MORIA_EDGE_HYPER, 0);

  free(Alist);
  free(Blist);
  identifier_list_free(B);
  identifier_list_free(A);
}

void operations_disconnect(moria_parser * restrict parser, identifier_list * restrict B, identifier_list * restrict A)
{
  moria_vertex **Blist;
  size_t Blen;
  size_t Bsz;
  moria_vertex **Alist;
  size_t Alen;
  size_t Asz;
  moria_edge *e;

  Blist = 0;
  Blen = 0;
  Bsz = 0;
  identifier_list_resolve(parser, B, &Blist, &Bsz, &Blen);

  Alist = 0;
  Alen = 0;
  Asz = 0;
  identifier_list_resolve(parser, A, &Alist, &Asz, &Alen);

  e = moria_edge_of(Blist, Blen, Alist, Alen);
  parser->dispatch->disconnect(parser, e);

  free(Blist);
  free(Alist);
  identifier_list_free(B);
  identifier_list_free(A);
}
