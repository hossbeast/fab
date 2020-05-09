# Summary

modules have a bunch of attributes which don't change and are simply declared

* use statements
* require statements
* variant declarations
* rule(s) to assign formulas to nodes

# event hooks

## rule-match

invoked when a rule is about to match

void bam_rule_match(

)

## rule-generate

invoked when a rule is about to generate a new node

## rule-connect

invoked when two nodes are about to be connected following rule evaluation

# module definitions

## IPC

simple / flexible / prototyping

each module is a separate program which runs in a subprocess and communicates with bamd
via signals-gated exchange of bacon messages

## embedded

fast


