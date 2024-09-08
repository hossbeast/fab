/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "bacon/value.h"
#include "bacon/make.h"
#include "bacon/parser.h"

#include "sysvar.h"
#include "build_slot.h"
#include "buildplan_entity.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "module.h"
#include "buildplan_entity.h"
#include "vertex.h"
#include "entlist.h"
#include "params.h"
#include "args.h"

//
// public
//

static void render_targets(
    bacon_value ** restrict vars
  , bacon_parser * restrict parser
  , const buildplan_entity * restrict bpe
  , const fsent * restrict base
)
{
  size_t count;
  const fsent *fse;
  entlist list;
  entlist *eln;
  bacon_value *key, *val, *mapping;
  char path[PATH_MAX];
  size_t z;
  int x;

  buildplan_entity_targets(bpe, &list);
  count = llist_count(&list.lln);

  bacon_string_mks(parser, 0, 0, &key, "bm_targets_len");
  bacon_string_mkf(parser, 0, 0, &val, "%zu", count);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, *vars, vars, mapping);

  x = 0;
  entlist_foreach(&list, fse, eln) {
    z = fsent_relative_path_cat(path, sizeof(path), fse, base);
    bacon_string_mkf(parser, 0, 0, &key, "bm_target%d", x++);
    bacon_string_mkw(parser, 0, 0, &val, path, z);
    bacon_mapping_mk(parser, 0, &mapping, key, val);
    bacon_set_mkv(parser, 0, *vars, vars, mapping);
  }

  entlist_destroy(&list);
}

static void render_sources(
    bacon_value ** restrict vars
  , bacon_parser * restrict parser
  , const buildplan_entity * restrict bpe
  , const fsent * restrict base
)
{
  size_t count;
  const fsent *fse;
  entlist list;
  entlist *eln;
  bacon_value *key, *val, *mapping;
  char path[PATH_MAX];
  size_t z;
  int x;

  buildplan_entity_sources(bpe, &list);
  count = llist_count(&list.lln);

  bacon_string_mks(parser, 0, 0, &key, "bm_sources_len");
  bacon_string_mkf(parser, 0, 0, &val, "%zu", count);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, *vars, vars, mapping);

  x = 0;
  entlist_foreach(&list, fse, eln) {
    z = fsent_relative_path_cat(path, sizeof(path), fse, base);
    bacon_string_mkf(parser, 0, 0, &key, "bm_source%d", x++);
    bacon_string_mkw(parser, 0, 0, &val, path, z);
    bacon_mapping_mk(parser, 0, &mapping, key, val);
    bacon_set_mkv(parser, 0, *vars, vars, mapping);
  }

  entlist_destroy(&list);
}

void sysvar_render(
    bacon_value ** restrict vars
  , bacon_parser * restrict parser
  , const build_slot * restrict bs
  , const fsent * restrict base
)
{
  bacon_value *key, *val, *mapping;
  const buildplan_entity * restrict bpe;

  bpe = bs->bpe;

  /* properties that pertain to this bpe */
  render_sources(vars, parser, bpe, base);
  render_targets(vars, parser, bpe, base);

  /* properties that pertain to this slot in the buildplan */
  bacon_string_mks(parser, 0, 0, &key, "bm_slot_index");
  bacon_string_mkf(parser, 0, 0, &val, "%"PRIu16, bs->stage_index);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, *vars, vars, mapping);

  /* properties that pertain to this bamd instance */
  bacon_string_mks(parser, 0, 0, &key, "bm_basedir");
  bacon_string_mkw(parser, 0, 0, &val, g_base_dir, g_base_dir_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, *vars, vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_homedir");
  bacon_string_mkw(parser, 0, 0, &val, g_params.homedir, g_params.homedir_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, *vars, vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_ipcdir");
  bacon_string_mkw(parser, 0, 0, &val, g_params.ipcdir, g_params.ipcdir_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, *vars, vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_hash");
  bacon_string_mkf(parser, 0, 0, &val, "0x%016"PRIx64, g_args.hash);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, *vars, vars, mapping);
}
