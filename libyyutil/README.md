# libyyutil

yyutil contains common implementation details for scanner/parser systems, including :

uniform logging output : tokens scanned, start condition changes, grammar and scanner errors

location tracking - provides a generic location struct including starting/ending
line/column/string and a macro for combining locations in parser productions

start condition management - provides a mechanism for storing a stack of start conditions in tandem
with but outside of flex which 1) does not make them accessible for logging purposes, and 2)
intermittently crashes when calling topstate

fatal macros for use within the parser - for things such as allocating memory
