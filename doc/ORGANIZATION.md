# project organization
* the project is organized into units, one per directory
* each unit may have its own unit tests, in the file test.c
* build flavors :
** 

# library project organization
* <name>.h contains the majority/essential public api
** additional public header files are at <name>/<unit>.h
* internal.h included in every source file
** #includes the public header file
** defines visibility constants API, APIDATA
** may re-define perrtab, the default error table

# binary project organization
* global.h is included by every source file
** contains #includes that are absolutely mandatory for every unit
** typically this is only logs.h and the appropriate errtab.h file
** should also re-define perrtab, the default error table
** because each #include creates a dependency for every unit, global.h #includes should be minimized


free/reset return void
dispose/recycle return int
