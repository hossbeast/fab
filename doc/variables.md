variant variables

copts.debug = -O3 -fno-builtin
copts.devel = -O2
cflags = -g $copts

%.o : %.c
{
  gcc $cflags -o $@ $^
}

fab.% : main.%.o
{
  gcc $cflags $lflags -o $@ $^
}

@all : fab.debug fab.devel fab.final
