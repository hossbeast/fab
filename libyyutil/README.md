# libyyutil

common implementation for scanner/parser systems, including,

logging probes - tokens scanned, start condition changes

error mesages - combines grammar and scanner errors

locations - generic location struct including starting/ending line/column/string and a macro for
combining locations in parser productions

start conditions - provides a mechanism for storing a stack of start conditions in tandem
with but outside of flex which 1) does not make them accessible for logging purposes, and 2)
intermittently crashes when calling topstate

scanning lvalues - functions and macros for scanning common types, such as integers and strings

scanning tokens - LEXA macro - allows for scanning tokens by their string value defined in the grammar
