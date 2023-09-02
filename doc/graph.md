# filesystem

# EDGE_FSTREE

the filesystem tree

edge : struct fsedge

digraph {
  A [label = "A : VERTEX_DIR" ]
  A -> { B C } [label = EDGE_FSTREE]
}

# EDGE_DEPENDS

the dependency graph

edge : struct dependency

digraph {
  A -> B [label = EDGE_DEPENDS]
}

# modules

## EDGE_IMPORTS

the module imports graph

edge : struct module_edge

digraph {
  A [label = "A : VERTEX_MODULE"]
  B [label = "B : VERTEX_DIR"]
  A -> B [label = EDGE_IMPORTS]
}

## EDGE_USES

the module uses graph

edge : struct module_edge

digraph {
  A [label = "A : VERTEX_MODULE"]
  B [label = "B : VERTEX_MODULE"]
  A -> B [label = EDGE_REQUIRES]
}

## EDGE_REQUIRES

the module requires graph

edge : struct module_edge

digraph {
  A [label = "A : VERTEX_MODULE"]
  B [label = "B : VERTEX_MODULE"]
  A -> B [label = EDGE_REQUIRES]
}

## EDGE_MOD_DIR

the filesystem subtree of module directory nodes
edge : struct moria_edge

digraph {
  A [label = "A : VERTEX_MODULE_DIR" ]
  B [label = "B : VERTEX_MODULE_DIR" ]
  A -> { B } [label = EDGE_MOD_DIR]
}

# rules

## EDGE_RULE_DIR

rule A matches in directory B

edge : struct rule_dirnode_edge

digraph {
  A [label = "A : VERTEX_RULE"]
  B [label = "B : VERTEX_DIR"]
  A -> B [label = "EDGE_RULE_DIR"]
}

## EDGE_MOD_RULE

rule B is run in context of module A

edge : struct rule_module_edge

digraph {
  A [label = "A : VERTEX_MODULE"]
  B [label = "B : VERTEX_RULE"]
  A -> B [label = "EDGE_MOD_RULE"]
}

## EDGE_RULE_FML

rule A's formula pattern matched formula B

edge : struct moria_edge

digraph {
  A [label = "A : VERTEX_RULE"]
  B [label = "B : VERTEX_FML"]
  A -> B [label = "EDGE_RULE_FML"]
}
