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

//
// public
//

xapi rules_apply(const list * restrict rules, node * restrict base, artifact * restrict af, int traversal_id)
{
  enter;

  int x, y, i;
  list * antecedent_list = 0;
  list * consequent_list = 0;

  fatal(list_create, &antecedent_list);
  fatal(list_create, &consequent_list);
  fatal(list_push, consequent_list, af->node);

  for(i = 0; i < consequent_list->l; i++)
  {
    node * consequent = list_get(consequent_list, i);
    vertex * consequent_vertex = vertex_containerof(consequent);

    if(consequent_vertex->visited == traversal_id)
      continue;
    consequent_vertex->visited = traversal_id;

    for(x = 0; x < rules->l; x++)
    {
      ff_node_rule * rule = list_get(rules, x);

      ff_node_pattern * consequent_pattern = rule->consequent_list->chain;
      while(consequent_pattern)
      {
        bool b;
        const char * stem;
        uint16_t stem_len;
        fatal(pattern_match, consequent_pattern, consequent, af, &b, &stem, &stem_len);
        if(b)
        {
          fatal(list_recycle, antecedent_list);
          
          ff_node_pattern * antecedent_pattern = rule->antecedent_list->chain;
          while(antecedent_pattern)
          {
            fatal(pattern_generate, antecedent_pattern, base, af, stem, stem_len, antecedent_list, false);
            antecedent_pattern = (typeof(antecedent_pattern))antecedent_pattern->next;
          }

          for(y = 0; y < antecedent_list->l; y++)
          {
            node * antecedent = list_get(antecedent_list, y);
            fatal(node_connect_dependency, consequent, antecedent);
            fatal(list_push, consequent_list, antecedent);
          }
        }

        consequent_pattern = (typeof(consequent_pattern))consequent_pattern->next;
      }
    }
  }

finally:
  fatal(list_xfree, antecedent_list);
  fatal(list_xfree, consequent_list);
coda;
}
