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

#include "graph.h"
#include "sweeper_thread.h"
#include "notify_thread.h"
#include "fsent_name.h"
#include "module.h"
#include "fsent_dir.h"
#include "fsent_symlink.h"

struct attrs32;
struct bacon_writer;
struct buildplan_entity;
struct config;
struct configset;
struct identity_ctx;
struct filesystem;
struct formula;
struct fsedge;
struct fstree;
struct graph_invalidation_context;
struct narrator;
struct plugin;
struct module;
struct rule_module_association;
struct varset;
struct edges;
struct graph_disintegrate_context;

enum moria_traversal_mode;

/* fsent special names */
#if CONFIG_DEVEL
#define FSENT_NAME_MODULE  "module.bam%devel"
#define FSENT_NAME_RULESET "rules.bam%devel"
#define FSENT_NAME_VARSET  "var.bam%devel"
#define FSENT_NAME_CONFIG  "config.bam%devel"
#else
#define FSENT_NAME_MODULE  "module.bam"
#define FSENT_NAME_RULESET "rules.bam"
#define FSENT_NAME_VARSET  "var.bam"
#define FSENT_NAME_CONFIG  "config.bam"
#endif

// globally scoped fsents
extern struct fsent *g_root;            // directory node at /
extern struct fsent *g_base;            // directory node at the base directory
extern char g_basedir[PATH_MAX];
extern uint16_t g_basedir_len;
extern int g_basedirfd;

// active list
extern llist fsent_list;

extern uint16_t fsent_fstree_epoch;       // when this changes, fsent <-> fstree mappings are discarded

/*
VERTEX_DIR
VERTEX_FILE
VERTEX_SYMLINK
VERTEX_SHADOW_DIR
VERTEX_SHADOW_FILE
VERTEX_SHADOW_LINK
VERTEX_MODULE_DIR
*/
typedef struct fsent {
  moria_vertex vertex;

  struct fsent_name name;           // node name (not the full path)
  char id[40];
  uint16_t id_len;

  /* for VERTEX_SHADOW_LINK nodes */
  uint32_t refmark;

  /* if set, then */
  /*  exists : bpe -> EDGE_BPE_TGT -> fsent (this) */
  /*  exists : fml -> EDGE_FML_BPE -> bpe */
  struct buildplan_entity *bpe;

  /* fsent reconcile
   *  dir : cleanup no-longer-referenced subtrees */
  llist unmount_lln;

  /* fsent reconcile
   *  reg : freshen invalidated primary nodes */
  llist freshen_lln;

  /* fsent reconcile
   *  reg: unlink primary files in managed build dirs */
  llist unlink_lln;

  /* filesystem reconcile
   *  dir : mkdirs */
  llist fs_mkdir_lln;

   /*  symlink : readlink */
  llist fs_readlink_lln;

  /* buildplan reconcile
   *  check for targets conflict */
  llist bpe_reconcile_lln;

  /* fsent_fstree_reconcile */
  llist notify_watched_files_lln;

  struct {                        // VERTEX_FILETYPE_DIR
    struct fstree *fst;           // not null
    uint16_t fstree_epoch;

    /* notify-thread */
    rbtree pending_child;         // sweeper_child_event
  };

  struct {                        // VERTEX_FILETYPE_REG or VERTEX_FILETYPE_SYMLINK
    uint16_t bp_plan_id;
    uint32_t hash;
  };

  stats_vertex stats;
  stats_fsent_dir dirstats;
  stats_fsent_symlink symlink_stats;

  /* readlink results */
  char *symlink_target;
  size_t symlink_target_sz;
  uint16_t symlink_target_len;

  int in_wd;                      // inotify watch descriptor
  uint32_t in_epoch;              // notify thread epoch
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
  DEF(FSENT_PROPERTY_ID           , "id"           , FSENT_PROPERTY_OPT, 0x9000) \
  /* other derived properties */                                                 \
  DEF(FSENT_PROPERTY_STATE        , "state"        , FSENT_PROPERTY_OPT, 0xc000) \
  DEF(FSENT_PROPERTY_TYPE         , "type"         , FSENT_PROPERTY_OPT, 0xd000) \


typedef enum fsent_property {
#undef DEF
#define DEF(x, s, r, y) x = UINT32_C(y),
FSENT_PROPERTY_TABLE
} fsent_property;

extern struct attrs32 *fsent_property_attrs;

typedef struct fsent_property_context {
  const struct fsent *cwd;
} fsent_property_context;

void fsent_root_setup(void);
void fsent_base_setup(void);
void fsent_cleanup(void);

/* apply config changes */
void fsent_reconfigure(struct config * restrict cfg, bool dryrun)
  __attribute__((nonnull));

void fsent_root_cleanup(void);

void fsent_create(
    fsent ** restrict np
  , vertex_kind kind
  , uint32_t attrs
  , const char * restrict label
  , uint16_t label_len
)
  __attribute__((nonnull));

void fsent_id_path_compute(const fsent * restrict fse, identity_ctx * restrict id)
  __attribute__((nonnull));

void fsent_compute_id(const fsent * restrict fse, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void fsent_compute_identifiers(fsent * restrict fse)
  __attribute__((nonnull));

void fsent_release(fsent * restrict n)
  __attribute__((nonnull));

static inline fsent *fsent_parent(const fsent * restrict n)
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
void fsent_graft(const char * restrict base, fsent ** restrict rn)
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
 * mark an fsent as VERTEX_FORMULA_FILE and reconcile its formula
 */
struct formula *fsent_formula_bootstrap(fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * mark an fsent as VERTEX_VARSET_FILE and reconcile its var
 */
struct varset *fsent_varset_bootstrap(fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * mark an fsent as VERTEX_CONFIGSET_FILE and reconcile its config blob
 */
void fsent_configset_bootstrap(fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * the delete event is fired when the node becomes unlinked though it may actually be released later
 */
void fsent_unlink(fsent * restrict n, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

fsent *fsent_translate(fsent * fse, const fsent *origin, const fsent *target, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void fsent_primary_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * mark an fsent to be considered for unlinking during next managed reconcile
 */
void fsent_unlink_candidate(fsent * restrict fse)
  __attribute__((nonnull));

/*
 * traverse managed build directories and clean up / delete all primary files
 */
void fsent_managed_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void fsent_modules_reconcile(void);

void fsent_fstree_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void fsent_freshen_candidate(fsent * restrict fse)
  __attribute__((nonnull));

void fsent_unmount_candidate(fsent * restrict fse)
  __attribute__((nonnull));

void fsent_account(void);

bool fsent_is_descendant_of(const fsent * restrict ancestor, const fsent * restrict n)
  __attribute__((nonnull));

bool fsent_is_self_or_descendant(const fsent * restrict ancestor, const fsent * restrict n)
  __attribute__((nonnull));

bool fsent_is_primary(const fsent * restrict fse)
  __attribute__((nonnull));

void fsent_namespace_refresh(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void fsent_fstree_refresh(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void fsent_dir_confirm(fsent * restrict fse)
  __attribute__((nonnull));

void fsent_promote_dir(fsent * restrict fse)
  __attribute__((nonnull));

void fsent_dir_describe(fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void fsent_symlink_describe(fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void fsent_describe(fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void fsent_stats_build(fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void fsent_id(const fsent * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

struct module *fsent_is_build_mod_file(const fsent * restrict fse)
  __attribute__((nonnull));

struct formula *fsent_is_formula_file(const fsent * restrict fse)
  __attribute__((nonnull));

struct configset *fsent_is_configset_file(const fsent * restrict fse)
  __attribute__((nonnull));

struct varset *fsent_is_varset_file(const fsent * restrict fse)
  __attribute__((nonnull));

struct ruleset *fsent_is_ruleset_file(const fsent * restrict fse)
  __attribute__((nonnull));

void fsents_show(void);
