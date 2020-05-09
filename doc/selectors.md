1. operations - input not used

# output: add these node(s) to the output set
module|target|all

# output: add these node(s) to the output set
lookup : foo/bar/main.o

2. filter operations

# for each input node, traverse, add visited nodes to the output
traverse : { direction : down }

# for each input node, add to the output if it matches the regex
regex : "a[bc]"

# for each input node, add to the output if it matches the pattern
pattern : a/b/c

# for each input node, 
odd

3. aggregates

# input to each inner operation is the input to the aggregate operation
# output of the aggregate operation is the union of outputs of the inner operations
union : [ ]

# input to each inner operation is the input to the aggregate operation
# output of the aggregate operation is the intersection of outputs of the inner operations
intersect : [ ]

# input to the first inner operation is the input to the aggregate operation
# input to each subsequent inner operations is the output from the preceding operation
# output of the aggregate operation is the output from the last inner operation
sequence : [ ]

sequence : [
  module
  traverse : { direction : up }
  traverse : { direction : down }
  union : [
    traverse : { direction : up }
    traverse : { direction : down }
  ]
]

sequence : [
  traverse : { direction : down }
  sequence : [
    traverse : { direction : up }
  ]
]

union : [
  module
  pattern : foo/bar/main.o
]
