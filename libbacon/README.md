Ba(m) con(figuration) object notation is a data model and structured text format similar to json.

In bacon, there is no object or dictionary type. Instead, mappings are a first-class type, and
objects are realized as a set of mappings.

scalar types
 boolean - true or false
 string - strings need not necessarily be quoted
 float - floating point number
 integer - signed integers
 variable - named, and referenced with the $ sigil

aggregate types
 set - unordered collection of unique values
 list - ordered collection of possibly duplicate values
 mapping - a pair of values, called the key and the value

# writer

write structured bacon text

# builder

helpers for properly using a bacon writer

# merge

combine two bacon values, in an add-only fashion

# clone

deep-copy bacon values

# coerce

convert bacon values to another type

# eval

evaluate expressions, with variables resolved against a collection of bacon values
