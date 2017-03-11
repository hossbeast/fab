# variables
env variables are passed to formulas at exec time
one of them is the variant name
variant variables are passed in resolved form


# inline
%.o : %.c priority 75 variables all
{
  $cc -o $target_stem.$target_suffix $cflags $transitive_deps
}

# standalone
#!/bin/bash
# %.o : %.c priority 75 variables all

$cc -o $target_stem.$target_suffix $cflags $transitive_deps
