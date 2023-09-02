Ideas for plugin integration

1. plugin that watches primary file invalidations. When any *.h file is invalidated, global invalidation
   or possibly invalidation across any module which imports the module the file belongs to

2. plugin that watches primary file invalidations. When any *.o file is refreshed, and that file is a
   primary file (not the target of any dependency edge), invalidates it instead. The idea is that any
   .o file ought to be the target of some edge to its .c file, or a hyperedge that fabricates the file
   from nothing
