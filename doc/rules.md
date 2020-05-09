# Summary

Rules create dependencies in the graph, and assign formulas to nodes.

Rules are written in one of the following forms,

 match-pattern <-> generate-pattern
 match-pattern <-> generate-pattern : formula
 generate-pattern : formula

Each rule is associated with one or more variants in the module. Rules declared at file-scope are
associated with all variants in the module.

## Patterns

There are two types of patterns,

1. Match-patterns

Match patterns support the following constructs

 *           - matches zero or more characters in the node name
 **          - matches the current directory and its subdirectories recursively, within in the module
 ()          - parenthesized groups
 (?<name>)   - parenthesized named groups
 +           - conjunction which matches if both the left and right sides match
 ~           - conjunction which matches if the left side matches and right side does not match

2. Generate-patterns

Generate patterns support the following constructs

 $n           - expands to a parenthesized group referenced by number
 $name        - expands to a parenthesized group referenced by name
 $^D          - expands to the filesystem directory of the node, if any
 $?           - expands to the variant
 $X:(s/a/b/o) - regex search/replace

All patterns permit the following constructs

 { a, b }  - alternation
 [ a - b ] - character class
 text      - literal text
 /         - directory separator
 ?         - variants associated with the rule

(in the initial position only)
 //module:  - the module (not in the filesystem)
 //modules: - this and child modules (not in the filesystem)

3. Reference-patterns

Reference patterns support the following constructs

 -none-

## Rule Evaluation

First, the match pattern is evaluated. Then, the generate pattern is evaluated once for each node
selected by the match pattern. If there is no match-pattern, the generate pattern is evaluated once.

Both the match and the generate patterns are evaluated from the context of the module base directory
node. That is, generated files by default are created in the base module directory. They can be made
relative to the directory containing the match node using $^D.

If the rule has a match pattern, then dependencies (RELATION-STRONG) are created for each evaluation
of the generate pattern. The directionality of these dependencies is determined by the direction of
the arrow between the patterns. The nodes pointed to by the arrow are said to be above, and to
depend on, the nodes pointed away from by the arrow, which are said to be below. That is, the arrow
points upward.

If the rule has a formula, then each evaluation of the generate pattern also yields a formula
instance, which is to be executed (a single time) in order to update the nodes pointed to by the
arrow.

### Rules and Variants

In general, a rule is evaluated only once.

 > the following has been superseded <

In general, a rule is evaluated once for each variant it is associated with. However, the following
caveats apply,

1. If the match pattern contains the ? token, then match pattern is evaluated only once.
2. If the match pattern does not contain the ? token, and the generate pattern does not contain the
   $? token, the match pattern is evaluated only once.
3. If the rule has no match pattern, and the generate pattern does not contain the $? token, the
   generate pattern is evaluated only once.
