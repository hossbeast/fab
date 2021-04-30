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

#ifndef FSENT_H
#define FSENT_H

/*

fsent objects are vertices in the graph which represent entities in the filesystem
like files, directories and symlink.

unlinked fsents are not matched by rules. uncreated fsents are matched by rules.

*/

#include "types.h"
#include "xapi.h"

#include "valyria/llist.h"
#include "valyria/rbtree.h"
#include "fab/stats.h"

#include "graph.h"
#include "sweeper_thread.h"
#include "notify_thread.h"
#include "fsname.h"

struct attrs32;
struct config;
struct configblob;
struct dependency;
struct filesystem;
struct formula;
struct fsedge;
struct fstree;
struct graph_invalidation_context;
struct map;
struct module;
struct narrator;
struct var;
struct variant;
struct channel;

enum moria_traversal_mode;

// specially named fsents
extern const char *fsent_model_name;
extern uint16_t fsent_model_name_len;
extern const char *fsent_module_name;
extern uint16_t fsent_module_name_len;
extern const char *fsent_var_name;
extern uint16_t fsent_var_name_len;
extern const char *fsent_config_name;
extern uint16_t fsent_config_name_len;

// globally scoped fsents
extern struct fsent * g_root;            // directory node at /
extern struct fsent * g_project_root;    // directory node at the project root
extern struct fsent * g_project_self;    // project module self node
extern struct fsent * g_project_shadow;  // project modules shadow node

// fsent lookup by inotify watch descriptor
extern struct map * g_fsent_by_wd;

// active list
extern llist fsent_list;

extern uint16_t fsent_valid_epoch;        // when this changes, all fsents are invalid
extern uint16_t fsent_module_epoch;       // when this changes, all fsent <-> module mappings are invalid

// VERTEX_TYPE_FSENT
typedef struct fsent {
  moria_vertex vertex;

  struct fsname name;               // name (not a full path)

  uint16_t valid_epoch;
  bool not_loaded;

  /* result of the most recent parse attempt */
  bool not_parsed;

  /* variant the node is associated with, if any */
  const struct variant * var;

  /* a node has at most one dependency via which it can be updated */
  struct dependency *dep;

  /* related edges */
  struct fsedge *import_scope_edge;
  struct fsedge *use_scope_edge;

  union {
    struct {                        // VERTEX_FILETYPE_DIR
      int wd;                       // watch descriptor
      bool descended;
      struct module * mod;          // module the directory belongs to (nearest ancestor)
      uint16_t module_epoch;

      const struct filesystem * fs; // not null
      const struct fstree *fst;     // not null if the fs is rooted at this node
      uint16_t fs_epoch;

      /* notify-thread */
      rbtree pending_child;         // sweeper_child_event
    };

    struct {                        // VERTEX_FILETYPE_REG
      uint16_t bp_plan_id;
      uint32_t hash;                // for INVALIDATE_STAT | INVALIDATE_CONTENT

      union {
        struct formula * self_fml;    // VERTEX_FORMULA_FILE     e.g. cc.bam
        struct var * self_var;        // VERTEX_VAR_FILE         var.bam
        struct config *self_config;   // VERTEX_CONFIG_FILE      e.g. config.bam (or /etc/fabconfig)
        struct {
          struct module * self_mod;   // VERTEX_MODULE_FILE      module.bam
          fab_module_file_stats modfile_stats;
        };
      };
    };
  };

  fab_fsent_stats stats;

  uint16_t ascend_walk_id;        // walker
  uint16_t descend_walk_id;       // walker
  sweeper_event pending;          // sweeper-thread
  enum notify_state notify_state;
  uint32_t notify_epoch;
} fsent;

#define FSENT_PROPERTY_OPT 0xf000
#define FSENT_PROPERTY_TABLE                                                     \
  /* direct properties of the node name */                                       \
  DEF(FSENT_PROPERTY_NAME         , "name"         , FSENT_PROPERTY_OPT, 0x1000) \
  DEF(FSENT_PROPERTY_EXT          , "ext"          , FSENT_PROPERTY_OPT, 0x2000) \
  DEF(FSENT_PROPERTY_SUFFIX       , "suffix"       , FSENT_PROPERTY_OPT, 0x3000) \
  DEF(FSENT_PROPERTY_BASE         , "base"         , FSENT_PROPERTY_OPT, 0x4000) \
  /* derived properties of the node path */                                      \
  DEF(FSENT_PROPERTY_ABSPATH      , "abspath"      , FSENT_PROPERTY_OPT, 0x5000) \
  DEF(FSENT_PROPERTY_ABSDIR       , "absdir"       , FSENT_PROPERTY_OPT, 0x6000) \
  DEF(FSENT_PROPERTY_RELPATH      , "relpath"      , FSENT_PROPERTY_OPT, 0x7000) \
  DEF(FSENT_PROPERTY_RELDIR       , "reldir"       , FSENT_PROPERTY_OPT, 0x8000) \
  /* other derived properties */                                                 \
  DEF(FSENT_PROPERTY_FSROOT       , "fsroot"       , FSENT_PROPERTY_OPT, 0x9000) \
  DEF(FSENT_PROPERTY_INVALIDATION , "invalidation" , FSENT_PROPERTY_OPT, 0xa000) \
  DEF(FSENT_PROPERTY_VARIANT      , "variant"      , FSENT_PROPERTY_OPT, 0xb000) \
  DEF(FSENT_PROPERTY_STATE        , "state"        , FSENT_PROPERTY_OPT, 0xc000) \
  DEF(FSENT_PROPERTY_TYPE         , "type"         , FSENT_PROPERTY_OPT, 0xd000) \

typedef enum fsent_property {
#undef DEF
#define DEF(x, s, r, y) x = UINT32_C(y),
FSENT_PROPERTY_TABLE
} fsent_property;

extern struct attrs32 * fsent_property_attrs;

typedef struct fsent_property_context {
  const struct module * mod;
} fsent_property_context;

xapi fsent_setup(void);
xapi fsent_setup_minimal(void);
xapi fsent_cleanup(void);

/* apply configuration changes */
xapi fsent_reconfigure(struct configblob * restrict cfg, bool dry)
  __attribute__((nonnull));

xapi fsent_create(
    fsent ** restrict np
  , vertex_kind kind
  , uint32_t attrs
  , const char * restrict label
  , uint16_t label_len
)
  __attribute__((nonnull));

void fsent_release(fsent * restrict n)
  __attribute__((nonnull));

static inline fsent * fsent_parent(const fsent * restrict n)
{
  const moria_vertex *v = &n->vertex;

  if((v = moria_vertex_up(v)))
    return containerof(v, fsent, vertex);

  return 0;
}

static inline enum vertex_kind fsent_kind_get(const fsent * restrict n)
{
  uint32_t attrs = n->vertex.attrs;
  attrs &= VERTEX_KIND_OPT;
  return attrs;
}

static inline void fsent_kind_set(fsent * restrict n, enum vertex_kind kind)
{
  uint32_t attrs = n->vertex.attrs;
  attrs &= ~VERTEX_KIND_OPT;
  attrs |= kind;

  RUNTIME_ASSERT(attrs);
  n->vertex.attrs = attrs;
}

static inline enum vertex_filetype fsent_filetype_get(const fsent * restrict n)
{
  uint32_t attrs = n->vertex.attrs;
  attrs &= VERTEX_FILETYPE_OPT;
  return attrs;
}

static inline void fsent_filetype_set(fsent * restrict n, enum vertex_filetype filetype)
{
  uint32_t attrs;

  attrs = n->vertex.attrs;

  attrs &= ~VERTEX_FILETYPE_OPT;
  attrs |= filetype;
  n->vertex.attrs = attrs;
}

static inline enum vertex_shadowtype fsent_shadowtype_get(const fsent * restrict n)
{
  uint32_t attrs = n->vertex.attrs;
  attrs &= VERTEX_SHADOWTYPE_OPT;
  return attrs;
}

static inline void fsent_shadowtype_set(fsent * restrict n, enum vertex_shadowtype shadowtype)
{
  uint32_t attrs = n->vertex.attrs;
  attrs &= ~VERTEX_SHADOWTYPE_OPT;
  attrs |= shadowtype;

  n->vertex.attrs = attrs;
}

static inline enum vertex_fsenttype fsent_fsenttype_get(const fsent * restrict n)
{
  uint32_t attrs = n->vertex.attrs;
  attrs &= VERTEX_FSENTTYPE_OPT;
  return attrs;
}

static inline enum vertex_type fsent_type_get(const fsent * restrict n)
{
  uint32_t attrs = n->vertex.attrs;
  attrs &= VERTEX_TYPE_OPT;
  return attrs;
}

static inline void fsent_fsenttype_set(fsent * restrict n, enum vertex_fsenttype fsenttype)
{
  uint32_t attrs = n->vertex.attrs;
  attrs &= ~VERTEX_FSENTTYPE_OPT;
  attrs |= fsenttype;

  n->vertex.attrs = attrs;

  if(fsenttype == VERTEX_FSENTTYPE_FML)
    n->self_fml = 0;
  else if(fsenttype == VERTEX_FSENTTYPE_VAR)
    n->self_var = 0;
  else if(fsenttype == VERTEX_FSENTTYPE_CONFIG)
    n->self_config = 0;
}

static inline void fsent_state_set(fsent * restrict n, vertex_state state)
{
  uint32_t attrs;

  attrs = n->vertex.attrs;
  attrs &= ~VERTEX_STATE_OPT;
  attrs |= state;

  RUNTIME_ASSERT(attrs);
  n->vertex.attrs = attrs;
}

static inline vertex_state fsent_state_get(fsent * restrict n)
{
  uint32_t attrs;

  attrs = n->vertex.attrs;
  attrs &= VERTEX_STATE_OPT;

  return attrs;
}

static inline bool fsent_exists_get(fsent * restrict n)
{
  uint32_t attrs;

  attrs = n->vertex.attrs;
  attrs &= VERTEX_EXISTS_BIT;

  return !!attrs;
}

static inline void fsent_exists_set(fsent * restrict n)
{
  uint32_t attrs;

  attrs = n->vertex.attrs;
  attrs |= VERTEX_EXISTS_BIT;

  n->vertex.attrs = attrs;
}

static inline bool fsent_invalid_get(fsent * restrict n)
{
  uint32_t attrs;

  attrs = n->vertex.attrs;
  attrs &= VERTEX_INVALID_BIT;
  if(attrs) {
    return true;
  }

  /* plain files with no dependencies are never invalid */
  if(fsent_kind_get(n) == VERTEX_FILE && !n->dep) {
    return false;
  }

  return n->valid_epoch != fsent_valid_epoch;
}

static inline void fsent_invalid_set(fsent * restrict n)
{
  uint32_t attrs;

  attrs = n->vertex.attrs;
  attrs &= ~VERTEX_INVALID_BIT;
  attrs |= VERTEX_INVALID_BIT;

  n->vertex.attrs = attrs;
}

static inline bool fsent_protect_get(fsent * restrict n)
{
  uint32_t attrs;

  attrs = n->vertex.attrs;
  attrs &= VERTEX_PROTECT_BIT;

  return !!attrs;
}

static inline void fsent_protect_set(fsent * restrict n)
{
  uint32_t attrs;

  attrs = n->vertex.attrs;
  attrs &= ~VERTEX_PROTECT_BIT;
  attrs |= VERTEX_PROTECT_BIT;

  n->vertex.attrs = attrs;
}

struct module * fsent_module_get(const fsent * restrict n)
  __attribute__((nonnull));

const struct filesystem * fsent_filesystem_get(fsent * restrict n)
  __attribute__((nonnull));

/*
 * get or create fsents starting at the root to some base path
 *
 * base - absolute path from the root to the desired fsent
 * rn   - (returns) fsent rooted at path base
 */
xapi fsent_graft(
    const char * restrict base
  , fsent ** restrict rn
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

xapi fsent_property_say(
    const fsent * restrict n
  , fsent_property property
  , const fsent_property_context * restrict ctx
  , struct narrator * restrict N
)
  __attribute__((nonnull));

size_t fsent_property_znload(
    void * restrict dst
  , size_t sz
  , const fsent * restrict n
  , fsent_property property
  , const fsent_property_context * restrict ctx
)
  __attribute__((nonnull));

/*
 * write the path to the fsent, relative to another fsent, to a buffer
 */
size_t fsent_relative_path_znload(void * restrict dst, size_t dst_size, const fsent * n, const fsent * base)
  __attribute__((nonnull));

/*
 * write the path to the fsent, relative to another fsent, to a buffer, encoded as a bacon string
 */
size_t fsent_relative_path_znload_bacon(void * restrict dst, size_t dst_size, const fsent * n, const fsent * base)
  __attribute__((nonnull));

/*
 * write the project-relative path to the fsent to a buffer
 */
size_t fsent_project_relative_path_znload(void * restrict dst, size_t sz, const fsent * restrict n)
  __attribute__((nonnull));

/*
 * write the module-relative path to the fsent to a buffer
 */
size_t fsent_module_relative_path_znload(void * restrict dst, size_t sz, const fsent * restrict n)
  __attribute__((nonnull));

/*
 * write the absolute path to the fsent to a buffer
 */
size_t fsent_absolute_path_znload(void * restrict dst, size_t sz, const fsent * restrict n)
  __attribute__((nonnull));

/*
 * absolute path for shadow nodes, project relative path otherwise
 */
size_t fsent_path_znload(void * restrict dst, size_t sz, const fsent * restrict n)
  __attribute__((nonnull));

/*
 * write the project-relative path to a fsent to a narrator
 */
xapi fsent_project_relative_path_say(const fsent * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write the module-relative path to a fsent to a narrator
 */
xapi fsent_module_relative_path_say(const fsent * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write the path to a fsent, relative to another fsent, to a narrator
 */
xapi fsent_relative_path_say(const fsent * n, const fsent * base, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write the absolute path to a fsent to a narrator
 */
xapi fsent_absolute_path_say(const fsent * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write one of the paths to a fsent to a narrator
 */
xapi fsent_path_say(fsent * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * get an fsent by traversing a path
 */
fsent *fsent_path_lookup(const struct module * restrict mod, const char * restrict path, uint16_t path_len)
  __attribute__((nonnull));

/*
 * mark an fsent as VERTEX_FORMULA_FILE and reconcile its formula
 */
xapi fsent_formula_bootstrap(fsent * restrict n, struct channel * restrict chan)
  __attribute__((nonnull));

/*
 * mark an fsent as VERTEX_VAR_FILE and reconcile its var
 */
xapi fsent_var_bootstrap(fsent * restrict n, struct channel * restrict chan)
  __attribute__((nonnull));

/*
 * mark an fsent as VERTEX_CONFIG_FILE and reconcile its config blob
 */
xapi fsent_config_bootstrap(fsent * restrict n, struct channel * restrict chan)
  __attribute__((nonnull));

/*
 * mark a node and its transitive consumers (upward) as invalid
 */
xapi fsent_invalidate(fsent * restrict n, struct graph_invalidation_context * restrict)
  __attribute__((nonnull));

xapi fsent_invalidate_visitor(moria_vertex * restrict v, void * arg, enum moria_traversal_mode mode, int distance, int * result)
__attribute__((nonnull(1, 2)));

/*
 * mark an fsent as up-to-date
 */
xapi fsent_ok(fsent * restrict n)
  __attribute__((nonnull));

/*
 * opportunistically delete related edges from this node - it is about to be unlinked from the fstree
 */
xapi fsent_unlink_related(fsent *n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * the delete event is fired when the node becomes unlinked though it may actually be released later
 */
xapi fsent_unlink(fsent *n, graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

xapi fsent_dirnode_children_changed(fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/* index a non-shadow directory node for graph lookup by label */
xapi fsent_lookup_index(fsent * restrict n)
  __attribute__((nonnull));

/* remove a previously indexed node from the graph lookup data structure */
xapi fsent_lookup_disindex(fsent * restrict n)
  __attribute__((nonnull));

#endif
