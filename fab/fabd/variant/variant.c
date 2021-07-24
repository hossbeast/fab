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

#include "xlinux/xstdlib.h"
#include "valyria/map.h"

#include "variant.h"
#include "zbuffer.h"

static map * variants_map;

//
// static
//

static void variant_free(variant * v)
{
  if(v) {
    wfree(v->tags);
  }

  wfree(v);
}

static int tag_compare(const void *A, const void *B)
{
  const struct variant_tag *a = A;
  const struct variant_tag *b = B;

  return memncmp(a->text, a->len, b->text, b->len);
}

static uint16_t normalize(
    const char * restrict name
  , uint16_t len
  , uint8_t * restrict num_labels
  , char * restrict dst
  , size_t dst_size
)
{
  size_t z;
  int x;
  bool dot;
  uint8_t nlabels = 0;
  const char * label_text = 0;
  struct variant_tag labels[32];

  dot = true;
  for(x = 0; x < len; x++)
  {
    if(name[x] == '.')
    {
      if(label_text && nlabels < (sizeof(labels) / sizeof(*labels)))
      {
        labels[nlabels].text = label_text;
        labels[nlabels].len = &name[x] - label_text;
        label_text = 0;
        nlabels++;
      }

      dot = true;
      continue;
    }
    if(!dot)
      continue;

    label_text = &name[x];
    dot = false;
  }

  if(label_text && nlabels < (sizeof(labels) / sizeof(*labels)))
  {
    labels[nlabels].text = label_text;
    labels[nlabels].len = &name[x] - label_text;
    nlabels++;
  }

  qsort(labels, nlabels, sizeof(*labels), tag_compare);

  (*num_labels) = 0;
  z = 0;
  for(x = 0; x < nlabels; x++)
  {
    // skip duplicates
    if(x && memncmp(labels[x - 1].text, labels[x - 1].len, labels[x].text, labels[x].len) == 0)
      continue;

    (*num_labels)++;

    if(x)
      z += znloadc(dst + z, dst_size - z, '.');

    z += znloadw(dst + z, dst_size - z, labels[x].text, labels[x].len);
  }

  return z;
}

static bool hastag(const variant * restrict var, const char *tag, uint16_t len)
{
  struct variant_tag key = { text : tag, len : len };

  if(var->tags)
    return bsearch(&key, var->tags, var->tags_len, sizeof(*var->tags), tag_compare);

  return false;
}

//
// public
//

void variant_get(const char * restrict name, uint16_t len, variant ** restrict variantp)
{
  variant * lv = 0;
  variant * v;
  char norm[256];
  uint16_t norm_len;

  uint8_t num_labels;

  int x;
  int i;

  // normalize
  norm_len = normalize(name, len, &num_labels, norm, sizeof(norm));

  if((v = map_get(variants_map, norm, norm_len)) == 0)
  {
    xmalloc(&lv, sizeof(*lv) + norm_len + 1);
    memcpy(lv->norm, norm, norm_len);
    lv->norm_len = norm_len;

    if(num_labels)
    {
      xmalloc(&lv->tags, sizeof(*lv->tags) * num_labels);
      x = 0;
      for(i = 0; i < norm_len; i++)
      {
        if(i == 0 || norm[i - 1] == '.')
        {
          lv->tags[x].text = &lv->norm[i];
          if(x)
            lv->tags[x - 1].len = (&lv->norm[i] - (lv->tags[x - 1].text)) - 1;
          x++;
        }
      }
      lv->tags[x - 1].len = (&lv->norm[i] - (lv->tags[x - 1].text));
      lv->tags_len = num_labels;
    }

    map_put(variants_map, norm, norm_len, lv, 0);
    v = lv;
    lv = 0;
  }

  *variantp = v;

finally:
  variant_free(lv);
coda;
}

bool variant_key_compatible(
    const variant * restrict var
  , const char * keytext
  , const char ** name
  , uint16_t * name_len
)
{
  const char *key[2];

  *name = 0;
  *name_len = 0;

  // if all tags on the definition are in the variants tags, the definition is compatible
  key[0] = key[1] = keytext;
  for(; *key[0]; key[1]++)
  {
    if(*key[1] != '.' && *key[1] != 0)
      continue;

    if(key[0] == keytext)
    {
      *name = key[0];
      *name_len = key[1] - key[0];
    }
    else if(!var || !hastag(var, key[0], key[1] - key[0]))
    {
      break;
    }

    key[0] = key[1];
    if(*key[0] == '.')
      key[0]++;
  }

  return *key[0] == 0;
}

void variant_setup()
{
  map_createx(&variants_map, 0, (void*)variant_free, 0);
}

void variant_cleanup()
{
  map_xfree(variants_map);
}
