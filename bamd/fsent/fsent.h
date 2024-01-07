/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

fsent objects are vertices in the graph which represent entities in the filesystem
like files, directories and symlink.

unlinked fsents are not matched by rules. uncreated fsents are matched by rules.

*/

#include "types.h"

#include "valyria/llist.h"
#include "valyria/rbtree.h"
#include "bam/stats.h"

#include "graph.h"
#include "sweeper_thread.h"
#include "notify_thread.h"
#include "fsname.h"
#include "module.h"

struct attrs32;
struct configuration;
struct buildplan_entity;
struct filesystem;
struct formula;
struct fsedge;
struct fstree;
struct graph_invalidation_context;
struct map;
struct narrator;
struct var;
struct bacon_writer;
struct rule_module_association;
struct bind_module_association;

enum moria_traversal_mode;

extern const char *fsent_name_module;
extern const char *fsent_name_rules;
extern const char *fsent_name_var;
extern const char *fsent_name_config;

// globally scoped fsents
extern struct fsent * g_root;            // directory node at /
extern struct fsent * g_base;             // directory node at the base directory
extern char g_base_path[PATH_MAX];
extern uint16_t g_base_path_len;
extern int g_base_dirfd;

// fsent lookup by inotify watch descriptor
extern struct map * g_fsent_by_wd;

// active list
extern llist fsent_list;

extern uint16_t fsent_valid_epoch;        // when this changes, all fsents are invalid
extern uint16_t fsent_module_epoch;       // when this changes, all fsent <-> module mappings are invalid

// VERTEX_TYPE_FSENT
typedef struct fsent {
  moria_vertex vertex;

  struct fsname name;               // node name (not the full path)
  char id[40];
  uint16_t id_len;
  char token[40];
  uint16_t token_len;

  uint64_t path_hash;               // hash over the full path to this node
  uint64_t life_token;              // increases on recycle

  /* cached */
  /* if set, then */
  /*  exists : bpe -> EDGE_BPE_TGT -> fsent (this) */
  bool is_target;

  /* if set, then */
  /*  exists : bpe -> EDGE_BPE_TGT -> fsent (this) */
  /*  exists : fml -> EDGE_FML_BPE -> bpe */
  struct buildplan_entity *bpe;

  /* if set, then */
  /*  exists : fsent (this) -> EDGE_INDEX -> index_vertex */
  struct index_edge *ie;

  /* related edges */
  struct fsedge *shadow_imports_edge;   // e.g. //module/imports/*
  struct fsedge *shadow_uses_edge;      // e.g. //module/uses/*
  struct fsedge *shadow_rules_edge;     // e.g. //module/uses/*
  struct fsedge *shadow_var_edge;       // e.g. //module/var/*

  /* directory : candidate for cleanup */
  llist prune_lln;

  /* dir : mkdirs
   * symlink : readlink */
  llist fs_reconcile_lln;

  llist bpe_reconcile_lln;

  union {
    struct {                        // VERTEX_FILETYPE_DIR
      bool descended;
      struct module *mod;           // module the directory belongs to (nearest ancestor)
      uint16_t module_epoch;

      const struct filesystem *fs;  // not null
      const struct fstree *fst;     // not null if the fs is rooted at this node
      uint16_t fs_epoch;
      uint16_t account_epoch;

      /* notify-thread */
      rbtree pending_child;         // sweeper_child_event
    };

    struct {                        // VERTEX_FILETYPE_REG
      uint16_t bp_plan_id;
      uint32_t content_hash;             // INVALIDATE_STAT | INVALIDATE_CONTENT

      union {
        struct formula * self_fml;          // VERTEX_FORMULA_FILE     e.g. cc.bam
        struct var * self_var;              // on VERTEX_VAR_FILE, the VERTEX_VAR
        struct config *self_config;         // VERTEX_CONFIG_FILE      e.g. config.bam
        struct rules *self_rules;           // VERTEX_RULES_FILE
                                            // VERTEX_SHADOW_MODULE
        struct module * self_mod;           // VERTEX_MODULE_FILE      module.bam
        struct rule_module_association *self_rma;  // VERTEX_SHADOW_RULE
        struct bind_module_association *self_bma;  // VERTEX_SHADOW_BIND
      };
    };
  };

  bam_vertex_stats stats;
  bam_fsent_dir_stats dirstats;

  int in_wd;                      // inotify watch descriptor
  uint32_t in_epoch;              // notify thread epoch
  uint16_t valid_epoch;           // fsent epoch
  uint16_t ascend_walk_id;        // walker
  uint16_t descend_walk_id;       // walker
  sweeper_event pending;          // sweeper-thread
  enum notify_state notify_state;
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
  DEF(FSENT_PROPERTY_STATE        , "state"        , FSENT_PROPERTY_OPT, 0xc000) \
  DEF(FSENT_PROPERTY_TYPE         , "type"         , FSENT_PROPERTY_OPT, 0xd000) \

typedef enum fsent_property {
#undef DEF
#define DEF(x, s, r, y) x = UINT32_C(y),
FSENT_PROPERTY_TABLE
} fsent_property;

extern struct attrs32 * fsent_property_attrs;

typedef struct fsent_property_context {
  const struct fsent *cwd;
} fsent_property_context;

void fsent_setup(void);
void fsent_system_bootstrap(void);
void fsent_cleanup(void);

/* apply configuration changes */
void fsent_reconfigure(struct configuration * restrict cfg, bool dryrun)
  __attribute__((nonnull));

void fsent_create(
    fsent ** restrict np
  , vertex_kind kind
  , uint32_t attrs
  , const char * restrict label
  , uint16_t label_len
)
  __attribute__((nonnull));

void fsent_compute_identifiers(fsent * restrict fse)
  __attribute__((nonnull));

void fsent_release(fsent * restrict n)
  __attribute__((nonnull));

struct module * fsent_module_get(const fsent * restrict n)
  __attribute__((nonnull));

const struct filesystem * fsent_filesystem_get(fsent * restrict n)
  __attribute__((nonnull));

static inline fsent * fsent_parent(const fsent * restrict n)
{
  const moria_vertex *v = &n->vertex;

  if((v = moria_vertex_up(v))) {
    return containerof(v, fsent, vertex);
  }

  return 0;
}

/*
 * get or create fsents starting at the root to some base path
 *
 * base - absolute path from the root to the desired fsent
 * rn   - (returns) fsent rooted at path base
 */
void fsent_graft(
    const char * restrict base
  , fsent ** restrict rn
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void fsent_property_say(
    const fsent * restrict n
  , fsent_property property
  , const fsent_property_context * restrict ctx
  , struct narrator * restrict N
)
  __attribute__((nonnull));

size_t fsent_property_cat(
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
size_t fsent_relative_path_cat(void * restrict dst, size_t dst_size, const fsent * n, const fsent * base)
  __attribute__((nonnull));

/*
 * write the path to the fsent, relative to another fsent, to a buffer, encoded as a bacon string
 */
size_t fsent_relative_path_cat_bacon(void * restrict dst, size_t dst_size, const fsent * n, const fsent * base)
  __attribute__((nonnull));

/*
 * write the cwd-relative path to the fsent to a buffer
 */
size_t fsent_base_relative_path_cat(void * restrict dst, size_t sz, const fsent * restrict n)
  __attribute__((nonnull));

void fsent_base_relative_path_describe(const fsent * restrict n, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

/*
 * write the absolute path to the fsent to a buffer
 */
size_t fsent_absolute_path_cat(void * restrict dst, size_t sz, const fsent * restrict n)
  __attribute__((nonnull));

void fsent_absolute_path_describe(const fsent * restrict n, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

/*
 * absolute path for shadow nodes, project relative path otherwise
 */
size_t fsent_path_cat(void * restrict dst, size_t sz, const fsent * restrict n)
  __attribute__((nonnull));

/*
 * write the project-relative path to a fsent to a narrator
 */
void fsent_base_relative_path_say(const fsent * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write the path to a fsent, relative to another fsent, to a narrator
 */
void fsent_relative_path_say(const fsent * n, const fsent * base, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write the absolute path to a fsent to a narrator
 */
void fsent_absolute_path_say(const fsent * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write one of the paths to a fsent to a narrator
 */
void fsent_path_say(fsent * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * get an fsent by traversing a path
 *
 * nofollow - whether to resolve symlinks
 */
fsent *fsent_path_lookup(
    const fsent * restrict base
  , const struct module * restrict mod
  , const char * restrict path
  , uint16_t path_len
  , bool nofollow
)
  __attribute__((nonnull(1, 3)));

/*
 * mark an fsent as VERTEX_FORMULA_FILE and reconcile its formula
 */
void fsent_formula_bootstrap(fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * mark an fsent as VERTEX_VAR_FILE and reconcile its var
 */
void fsent_var_bootstrap(fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * mark an fsent as VERTEX_CONFIG_FILE and reconcile its config blob
 */
void fsent_config_bootstrap(fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * the delete event is fired when the node becomes unlinked though it may actually be released later
 */
void fsent_unlink(fsent *n, struct entlist * restrict edges, graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

fsent *fsent_translate(fsent * fse, const fsent *origin, const fsent *target, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

size_t fsent_dir_stats_collate(void *dst, size_t sz, bool reset, fsent * restrict n)
  __attribute__((nonnull));

void fsent_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void fsent_prune_candidate(fsent * restrict fse)
  __attribute__((nonnull));

void fsent_account(void);

bool fsent_is_descendant_of(const fsent * restrict ancestor, const fsent * restrict n)
  __attribute__((nonnull));

bool fsent_is_self_or_descendant(const fsent * restrict ancestor, const fsent * restrict n)
  __attribute__((nonnull));

/* returns true if n lies within one of the mount points */
bool fsent_is_mounted(const fsent * restrict n)
  __attribute__((nonnull));

uint64_t fsent_abspath_hash(const fsent * restrict fse)
  __attribute__((nonnull));

void fsent_fstree_refresh(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void fsent_promote_dir(fsent * restrict fse)
  __attribute__((nonnull));
