# libxlinux

libxlinux provides xapi wrappers, called proxies, for standard linux calls (open, read, etc)

## Function Signatures

The function call exactly matches the underlying function, unless the return value
of the underlying function may be useful to the caller, in which case it is returned
via the final parameter to the proxy

## Naming Convention

if a function has both a Type and Operation modifier, the Operation modifier is first

### Grammar

fname
  : Operations Type name
  | Operations name
  | Type name

Operations
  : Operations Operation
  | Operation
  
Operation
  : 'i'
  | 'u'
  | 'a'
  | 'e'

Type
  : 'w'
  | 'x'

### Type

w (wrapper)
 non-xapi wrapper function that follows the zero-return-success model and fails exactly
 when the underlying function fails

x (xapi)
 xapi-enabled proxy function that fails in exactly the same cases as the underlying function

#### Operation

i (idempotent)
 accepts a pointer to an identifier to some resource and, upon successful completion, sets
 the value pointed to to the identity value for that resource type

u (union)
 fails only in some subset of failure cases of the underlying function as specified

a (also)
 fails in some superset of failure cases of the underlying function as specified

e (extended)
 performs the same operation as the underlying function on another data type
