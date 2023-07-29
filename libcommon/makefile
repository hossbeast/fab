CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common

CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

CFLAGS += -I.
CFLAGS += -Ifmt
CFLAGS += -Ipermute
CFLAGS += -Igrow
CFLAGS += -Iparseint
CFLAGS += -Isnarf
CFLAGS += -Istresc
CFLAGS += -Icommon
CFLAGS += -Iassure
CFLAGS += -Icolor
CFLAGS += -Ihash
CFLAGS += -Iattrs
CFLAGS += -I../libcommon
CFLAGS += -I../include
CFLAGS += -I../libxapi
CFLAGS += -I../libxlinux

obj += fmt/fmt.o
obj += permute/permute.o
obj += grow/grow.o
obj += parseint/parseint.o
obj += snarf/snarf.o
obj += stresc/stresc.o
obj += assure/assure.o
obj += color/color.o
obj += hash/hash.o
obj += attrs/attrs.o

libcommon.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
