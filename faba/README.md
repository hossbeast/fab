faba (fab agent)
===

The fab agent is a long-running process that carries out tasks for which that
require the users credentials, such as reading the fabfiles, and executing
build tasks which may create / write to files in user directories

When the user re-executes fab with different credentials, the faba process
is recycled

duties:
r  - executes discovery formulas
    - produces lists of dependencies
   - executes buildplan formulas
 w  - produces secondary files
 w - writes the users buildscript file
r  - reads fabfiles
r   - stathashes fabfiles
r  - monitors primary files with inotify (which?)
r  - stathashes primary files on demand
