/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "bacon/value.h"
#include "bacon/make.h"
#include "bacon/parser.h"

#include "sysvar.h"
#include "build_slot.h"
#include "bpe.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "module.h"
#include "rma.h"
#include "vertex.h"
#include "edges.h"
#include "vertices.h"
#include "params.h"
#include "args.h"
#include "formula_record.h"
#include "build_record.h"
#include "git-state.h"

static bacon_parser *sysvar_parser;
bacon_value *sysvar_globals;

//
// public
//

void sysvar_setup(void)
{
  bacon_parser_alloc(&sysvar_parser);

  sysvar_globals = sysvar_global_render(sysvar_parser, 0);
}

void sysvar_cleanup(void)
{
  bacon_value_free(sysvar_parser, sysvar_globals);
  bacon_parser_free(sysvar_parser);
}

bacon_value *sysvar_global_render(bacon_parser * restrict parser, bacon_value *vars)
{
  bacon_value *key, *val, *mapping;

  /* global sysvars - properties that pertain to this bamd instance */
  bacon_string_mks(parser, 0, 0, &key, "bm_basedir");
  bacon_string_mkw(parser, 0, 0, &val, g_basedir, g_basedir_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_homedir");
  bacon_string_mkw(parser, 0, 0, &val, g_params.homedir, g_params.homedir_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_ipcdir");
  bacon_string_mkw(parser, 0, 0, &val, g_params.ipcdir, g_params.ipcdir_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_bamd_id");
  bacon_string_mkw(parser, 0, 0, &val, g_args.bamd_id, g_args.bamd_id_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_bamd_pid");
  bacon_integer_mk(parser, 0, &val, g_params.pid);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_git_describe");
  bacon_string_mks(parser, 0, 0, &val, git_describe);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  return vars;
}

bacon_value *sysvar_module_render(bacon_parser * restrict parser, bacon_value *vars, const module * restrict mod)
{
  bacon_value *key, *val, *mapping;
  char path[PATH_MAX];
  size_t path_len;

  /* module sysvars - under the module for the slot */
  bacon_string_mks(parser, 0, 0, &key, "bm_module_id");
  bacon_string_mkw(parser, 0, 0, &val, mod->id, mod->id_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_module_name");
  bacon_string_mkw(parser, 0, 0, &val, mod->name, mod->name_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  path_len = fsent_absolute_path_cat(path, sizeof(path), mod->build_dir);
  bacon_string_mks(parser, 0, 0, &key, "bm_module_build_dir");
  bacon_string_mkw(parser, 0, 0, &val, path, path_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_module_build_dir_name");
  bacon_string_mkw(parser, 0, 0, &val, mod->build_dir->vertex.label, mod->build_dir->vertex.label_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  path_len = fsent_absolute_path_cat(path, sizeof(path), mod->anchor_dir);
  bacon_string_mks(parser, 0, 0, &key, "bm_module_anchor_dir");
  bacon_string_mkw(parser, 0, 0, &val, path, path_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_module_anchor_dir_name");
  bacon_string_mkw(parser, 0, 0, &val, mod->anchor_dir->vertex.label, mod->anchor_dir->vertex.label_len);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  return vars;
}
