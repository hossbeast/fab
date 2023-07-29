CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common
CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

CFLAGS += -I.
CFLAGS += -Iload
CFLAGS += -Iprobes
CFLAGS += -Ioperations
CFLAGS += -Ioperations
CFLAGS += -Itraverse
CFLAGS += -Itraverse
CFLAGS += -Igraph
CFLAGS += -Igraph
CFLAGS += -Iedge
CFLAGS += -Iparser
CFLAGS += -Iparser
CFLAGS += -Imoria
CFLAGS += -Iattr
CFLAGS += -Ilogging
CFLAGS += -Ivertex
CFLAGS += -Ivertex
CFLAGS += -Ierrtab
CFLAGS += -I../libcommon
CFLAGS += -I../include
CFLAGS += -I../libxapi
CFLAGS += -I../libxlinux
CFLAGS += -I../libnarrator
CFLAGS += -I../libvalyria
CFLAGS += -I../liblogger
CFLAGS += -I../libyyutil

obj += load/load.o
obj += probes/probes.o
obj += operations/operations.o
obj += traverse/traverse.o
obj += graph/graph.o
obj += edge/edge.o
obj += parser/parser.o
obj += logging/logging.o
obj += vertex/vertex.o
obj += errtab/MORIA.errtab.o

libmoria.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
