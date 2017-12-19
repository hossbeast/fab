/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#include <string.h>

#include "xapi.h"
#include "types.h"
#include "narrator.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "moria/vertex.h"

#include "rule.internal.h"
#include "ff_node.internal.h"
#include "ff_node_pattern.h"
#include "ff_node_patterns.h"
#include "ff_node_rule.internal.h"
#include "node.h"
#include "node_operations.h"
#include "path.h"
#include "match.h"
#include "generate.h"
#include "artifact.h"
#include "module.h"

//
// internal
//


xapi rules_apply_rules(
    node * restrict consequent
  , const list * restrict rules_lists
  , node * restrict base
  , map * restrict scope
  , artifact * restrict af
  , list * restrict antecedents
  , list * restrict consequents
)
{
  enter;

  int x, y, z;

  for(x = 0; x < rules_lists->l; x++)
  {
    const list * rules = list_get(rules_lists, x);
    for(y = 0; y < rules->l; y++)
    {
      ff_node_rule * rule = list_get(rules, y);

      ff_node_pattern * consequent_pattern = rule->consequent_list->chain;
      while(consequent_pattern)
      {
        bool b;
        const char * stem;
        uint16_t stem_len;
        fatal(pattern_match, consequent_pattern, consequent, af, &b, &stem, &stem_len);
        if(b)
        {
          fatal(list_recycle, antecedents);
          
          ff_node_pattern * antecedent_pattern = rule->antecedent_list->chain;
          while(antecedent_pattern)
          {
            fatal(pattern_generate, antecedent_pattern, base, scope, af, stem, stem_len, antecedents, false);
            antecedent_pattern = (typeof(antecedent_pattern))antecedent_pattern->next;
          }

          for(z = 0; z < antecedents->l; z++)
          {
            node * antecedent = list_get(antecedents, z);

            fatal(node_connect, consequent, antecedent, RELATION_TYPE_STRONG);
            fatal(list_push, consequents, antecedent);
          }
        }

        consequent_pattern = (typeof(consequent_pattern))consequent_pattern->next;
      }
    }
  }

  finally : coda;
}

//
// public
//

xapi rules_apply(artifact * restrict af, int traversal_id)
{
  enter;

  int x;
  list * antecedents = 0;
  list * consequents = 0;

  char path[512];
  size_t pathl;

  fatal(list_create, &antecedents);
  fatal(list_create, &consequents);
  fatal(list_push, consequents, af->node);

  for(x = 0; x < consequents->l; x++)
  {
    node * consequent = list_get(consequents, x);
    vertex * consequent_vertex = vertex_containerof(consequent);

    if(consequent_vertex->visited == traversal_id)
      continue;
    consequent_vertex->visited = traversal_id;

    // on-demand module attribution
    if(!consequent->mod)
    {
      pathl = node_get_absolute_path(consequent, path, sizeof(path));
      consequent->mod = module_lookup(path, pathl);
    }

    if(!consequent->mod)
      continue;

    const list * rules_lists = consequent->mod->rules_lists;
    node * base = consequent->mod->base;
    map * scope = consequent->mod->require_scope;

    fatal(rules_apply_rules, consequent, rules_lists, base, scope, af, antecedents, consequents);
  }

finally:
  fatal(list_xfree, antecedents);
  fatal(list_xfree, consequents);
coda;
}
