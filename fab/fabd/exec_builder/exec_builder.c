/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#include <stdio.h>

#include "value.h"
#include "xlinux/xstdlib.h"

#include "exec_builder.h"
#include "exec.h"
#include "formula_value.internal.h"
#include "path_cache.h"

#include "common/assure.h"
#include "common/attrs.h"

//
// static
//

static void render_say(const char * restrict name, enum builder_render_function renderer, const union builder_render_function_args *args, narrator * restrict N)
{
  if(renderer == BUILDER_PROPERTY)
  {
    xsayf("render-%s PROPERTY\n", name);
    xsayf(" node %p %s\n", args->n, args->n->name.name);
    xsayf(" prop %s\n", attrs32_name_byvalue(fsent_property_attrs, FSENT_PROPERTY_OPT & args->prop));
  }
  else if(renderer == BUILDER_PATH_CACHE_ENTRY)
  {
    xsayf("render-%s PATH-CACHE-ENTRY\n", name);
  }
  else if(renderer == BUILDER_STRING)
  {
    xsayf("render-%s STRING\n", name);
    xsayf(" string '%s'\n", args->s);
  }
  else if(renderer == BUILDER_FORMULA_VALUE)
  {
    xsayf("render-%s FORMULA-VALUE\n", name);
    xsays(" formula-value ");
    formula_value_say(args->f, N);
    xsays("\n");
  }
  else if(renderer == BUILDER_VALUE)
  {
    xsayf("render-%s VALUE\n", name);
    xsays(" value ");
    value_say(args->v, N);
    xsays("\n");
  }
  else if(renderer == BUILDER_FMT)
  {
    xsayf("render-%s FMT\n", name);
    xsayf(" fmt %s\n", args->fmt);
    xsayf(" va %p\n", args->va);
  }
}

struct def_name_compare_context {
  const char * textbase;
  const uint32_t * base;
  const char * name;  // key to find

  int lx; // last compare position
  int lc; // last compare result
};

int env_def_cmp(const char * restrict name, const char * restrict text)
{
  int r;

  while(name && text)
  {
    if((r = INTCMP(*name, *text)))
      return r;

    name++;
    if(!*name || *name == '=')
      name = 0;

    text++;
    if(!*text || *text == '=')
      text = 0;
  }

  if(!name && !text)
    return 0;

  return INTCMP(name, text);
}

static int def_name_compare(const void * _ctx, const void * _item)
{
  struct def_name_compare_context * ctx = (typeof(ctx))_ctx;

  const uint32_t * item = _item;

  ctx->lx = item - ctx->base;
  return ctx->lc = env_def_cmp(ctx->name, ctx->textbase + *item);
}

static void __attribute__((nonnull)) dispatch_render_function(
    enum builder_render_function func
  , const union builder_render_function_args * restrict args
  , narrator * restrict N
)
{
  const path_cache_entry *pe;

  if(func == BUILDER_PROPERTY)
  {
    fsent_property_say(args->n, args->prop, &args->pctx, N);
  }
  else if(func == BUILDER_STRING)
  {
    narrator_xsays(N, args->s);
  }
  else if(func == BUILDER_FORMULA_VALUE)
  {
    formula_value_render(args->f, N);
  }
  else if(func == BUILDER_VALUE)
  {
    value_render(args->v, N);
  }
  else if(func == BUILDER_FMT)
  {
    narrator_xsayvf(N, args->fmt, *args->va);
  }

  if(func != BUILDER_PATH_CACHE_ENTRY) {
    return;
  }

  pe = args->pe;
  if(pe->dir) {
    narrator_xsayw(N, pe->dir->s, pe->dir->len);
    narrator_xsays(N, "/");
  }

  narrator_xsayw(N, pe->s, pe->len);
}

void exec_builder_add(exec_builder * restrict builder, const exec_builder_args *args)
{
  uint32_t def;
  uint32_t *defp;
  char * textbase;
  struct def_name_compare_context ctx;
  off_t pos;
  uint32_t start;
  uint32_t idx;
  size_t len;
  const char *eq;

  /* special case of rendering a path entry to the file - save the pe so that exec can use the fd */
  if(args->item == BUILDER_FILE && args->render_val == BUILDER_PATH_CACHE_ENTRY)
  {
    builder->file_pe = args->val.pe;
    return;
  }

  // current narrator position
  narrator_xseek(builder->Nexec, 0, NARRATOR_SEEK_CUR, &pos);
  start = pos;

  def = 0;
  if(args->item == BUILDER_FILE)
  {
    // locate an existing definition if any
    def = builder->file;

    // new definition goes here
    builder->file = start;
  }
  else if(args->item == BUILDER_ARGS)
  {
    // check for an existing definition
    if(args->position > -1)
    {
      def = builder->args[args->position];
      idx = args->position;
    }
    else
    {
      idx = builder->args_len++;
      assure(&builder->args, sizeof(*builder->args), idx + 1, &builder->args_alloc);
    }

    builder->args[idx] = start;
  }
  else if(args->item == BUILDER_ENVS)
  {
    // search for an existing definition
    memset(&ctx, 0, sizeof(ctx));
    ctx.lc = -1;
    if(args->position > -1)
    {
      def = builder->envs[args->position];
      ctx.lc = 0;
      ctx.lx = args->position;
    }
    else if(builder->envs && args->name)
    {
      textbase = builder->Nexec_growing.s;
      ctx.textbase = textbase;
      ctx.base = builder->envs;
      ctx.name = args->name;
      defp = bsearch(&ctx, builder->envs, builder->envs_len, sizeof(*builder->envs), def_name_compare);
      if(defp)
        def = *defp;
    }

    // displacement if no existing definition
    idx = ctx.lx;
    if(ctx.lc)
    {
      if(ctx.lc > 0)
        idx++;

      assure(&builder->envs, sizeof(*builder->envs), builder->envs_len + 1, &builder->envs_alloc);
      memmove(
          builder->envs + (idx + 1)
        , builder->envs + idx
        , (builder->envs_len - idx) * sizeof(*builder->envs)
      );

      builder->envs_len++;
    }

    builder->envs[idx] = start;
  }

  if(args->mode == BUILDER_PREPEND)
  {
    if(args->item == BUILDER_ENVS)
    {
      narrator_xsayw(builder->Nexec, args->name, args->name_len);
      narrator_xsays(builder->Nexec, "=");
    }

    // append the value from this definition
    dispatch_render_function(args->render_val, &args->val, builder->Nexec);

    if(def)
    {
      if(args->render_sep) {
        dispatch_render_function(args->render_sep, &args->sep, builder->Nexec);
      }

      // carry forward the existing definition
      textbase = builder->Nexec_growing.s;
      if(args->item == BUILDER_ENVS)
      {
        eq = strchr(textbase + def, '=');
        len = strlen(eq + 1);
        narrator_growing_allocate(&builder->Nexec_growing, len);
        textbase = builder->Nexec_growing.s;
        eq = strchr(textbase + def, '=');
        narrator_xsayw(builder->Nexec, eq + 1, len);
      }
      else
      {
        len = strlen(textbase + def);
        narrator_growing_allocate(&builder->Nexec_growing, len);
        textbase = builder->Nexec_growing.s;
        narrator_xsayw(builder->Nexec, textbase + def, len);
      }
    }

    narrator_xsayc(builder->Nexec, 0);
  }
  else if(args->mode == BUILDER_APPEND)
  {
    if(def)
    {
      // carry forward the existing definition
      textbase = builder->Nexec_growing.s;
      len = strlen(textbase + def);
      narrator_growing_allocate(&builder->Nexec_growing, len);
      textbase = builder->Nexec_growing.s;
      narrator_xsayw(builder->Nexec, textbase + def, len);
    }
    else if(args->item == BUILDER_ENVS)
    {
      narrator_xsayw(builder->Nexec, args->name, args->name_len);
      narrator_xsays(builder->Nexec, "=");
    }

    if(def && args->render_sep) {
      dispatch_render_function(args->render_sep, &args->sep, builder->Nexec);
    }

    // append the value from this definition
    dispatch_render_function(args->render_val, &args->val, builder->Nexec);
    narrator_xsayc(builder->Nexec, 0);
  }
  else
  {
    RUNTIME_ABORT();
  }
}

void exec_builder_env_addf(exec_builder * restrict builder, const char * restrict name_fmt, const char * restrict val_fmt, ...)
{
  va_list va;
  char name[64];
  size_t namel;
  struct exec_builder_args args = { };

  va_start(va, val_fmt);

  namel = vsnprintf(name, sizeof(name), name_fmt, va);

  args.item = BUILDER_ENVS;
  args.position = -1;
  args.name = name;
  args.name_len = namel;
  args.render_val = BUILDER_FMT;
  args.val.fmt = val_fmt;
  args.val.va = &va;
  args.mode = BUILDER_APPEND;

  args.render_sep = BUILDER_STRING;
  args.sep.s = " ";
  exec_builder_add(builder, &args);
}

//
// tracing
//

void exec_builder_args_say(const exec_builder * restrict builder, const exec_builder_args * restrict args, narrator * restrict N)
  __attribute__((nonnull));

void exec_builder_args_say(const exec_builder * restrict builder, const exec_builder_args * restrict args, narrator * restrict N)
{
  xsayf("builder %p\n", builder);
  xsayf("item %s\n", args->item == BUILDER_FILE ? "FILE" : args->item == BUILDER_ARGS ? "ARGS" : args->item == BUILDER_ENVS ? "ENVS" : "-wtf-");
  xsayf("position %d\n", args->position);
  xsayf("name %.*s\n", (int)args->name_len, args->name);
  xsayf("name-len %hu\n", args->name_len);
  xsayf("mode %d %s\n", args->mode, args->mode == BUILDER_PREPEND ? "PREPEND" : args->mode == BUILDER_APPEND ? "APPEND" : "-wtf-");
  render_say("value", args->render_val, &args->val, N);
  render_say("sep", args->render_sep, &args->sep, N);
}

//
// public
//

void exec_builder_take(exec_builder * restrict builder, exec ** restrict envp)
{
  size_t exec_size;

  narrator_growing_claim_buffer(&builder->Nexec_growing, &builder->exec, &exec_size);

  *envp = builder->exec;
  builder->exec = 0;

  builder->file_stor = 0;
  builder->args_stor = 0;
  builder->args_stor_alloc = 0;
  builder->envs_stor = 0;
  builder->envs_stor_alloc = 0;
}

void exec_free(exec * e)
{
  if(e)
  {
    wfree(e->args);
    wfree(e->envs);
  }
  wfree(e);
}

void exec_ifree(exec ** restrict ep)
{
  if(*ep)
    exec_free(*ep);

  *ep = 0;
}

void exec_builder_xinit(exec_builder * restrict builder)
{
  memset(builder, 0, sizeof(*builder));

  builder->Nexec = narrator_growing_init(&builder->Nexec_growing);

  // position immediately after the env struct
  narrator_xseek(builder->Nexec, sizeof(exec), NARRATOR_SEEK_SET, 0);
}

void exec_builder_xreset(exec_builder * restrict builder)
{
  builder->file_pe = 0;
  builder->file = 0;
  builder->envs_len = 0;
  builder->args_len = 0;

  // position immediately after the env struct
  narrator_xseek(builder->Nexec, sizeof(exec), NARRATOR_SEEK_SET, 0);
}

void exec_builder_xdestroy(exec_builder * restrict builder)
{
  narrator_growing_destroy(&builder->Nexec_growing);
  wfree(builder->args);
  wfree(builder->args_stor);
  wfree(builder->envs);
  wfree(builder->envs_stor);
}

void exec_builder_build(exec_builder * restrict builder, exec ** restrict envp)
{
  int x;
  char * textbase;

  textbase = builder->Nexec_growing.s;
  if(textbase == 0)
  {
    /* nothing has been added to the builder */
    narrator_growing_allocate(&builder->Nexec_growing, sizeof(exec));
    textbase = builder->Nexec_growing.s;
  }

  builder->exec = (typeof(builder->exec))textbase;
  RUNTIME_ASSERT(builder->exec);

  // file - pointer from offset
  builder->exec->file_pe = builder->file_pe;
  builder->exec->path = 0;
  if(builder->file)
  {
    builder->file_stor = textbase + builder->file;
    builder->exec->path = builder->file_stor;
  }

  // args - space
  if(builder->args_stor_alloc < (builder->args_len + 1))
  {
    xrealloc(&builder->args_stor, sizeof(*builder->args_stor), builder->args_len + 1, builder->args_stor_alloc);
    builder->args_stor_alloc = builder->args_len + 1;
  }

  // args- pointers from offsets
  for(x = 0; x < builder->args_len; x++)
    builder->args_stor[x] = textbase + builder->args[x];
  builder->args_stor[x] = 0;

  builder->exec->args = builder->args_stor;
  builder->exec->args_size = builder->args_len;

  // envs - space
  if(builder->envs_stor_alloc < (builder->envs_len + 1))
  {
    xrealloc(&builder->envs_stor, sizeof(*builder->envs_stor), builder->envs_len + 1, builder->envs_stor_alloc);
    builder->envs_stor_alloc = builder->envs_len + 1;
  }

  // envs - pointers from offsets
  for(x = 0; x < builder->envs_len; x++)
    builder->envs_stor[x] = textbase + builder->envs[x];
  builder->envs_stor[x] = 0;

  builder->exec->envs = builder->envs_stor;
  builder->exec->envs_size = builder->envs_len;

  // return
  if(envp)
  {
    *envp = builder->exec;
  }
}
