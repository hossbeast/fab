/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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

//
// public
//

static void render_targets(
    bacon_value ** restrict vars
  , bacon_parser * restrict parser
  , const buildplan_entity * restrict bpe
  , const fsent * restrict cwd
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
    z = fsent_relative_path_cat(path, sizeof(path), fse, cwd);
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
  , const fsent * restrict cwd
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
    z = fsent_relative_path_cat(path, sizeof(path), fse, cwd);
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
  , const buildplan_entity * restrict bpe
  , const fsent * restrict cwd
)
{
  render_sources(vars, parser, bpe, cwd);
  render_targets(vars, parser, bpe, cwd);
}
