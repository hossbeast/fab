CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common

CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

CFLAGS += -I.
CFLAGS += -Iload
CFLAGS += -Irequest_writer
CFLAGS += -Igraph
CFLAGS += -Iipc
CFLAGS += -Istats
CFLAGS += -Isigutil
CFLAGS += -Ifab
CFLAGS += -Ilist
CFLAGS += -Ilockfile
CFLAGS += -Ievents
CFLAGS += -Ibuild_slot
CFLAGS += -Ifsent
CFLAGS += -Ilogging
CFLAGS += -Iclient
CFLAGS += -Imetadata
CFLAGS += -Ierrtab
CFLAGS += -Idescribe
CFLAGS += -I../../libcommon
CFLAGS += -I../../include
CFLAGS += -I../../libxapi
CFLAGS += -I../../libxlinux
CFLAGS += -I../../libnarrator
CFLAGS += -I../../libvalyria
CFLAGS += -I../../liblogger
CFLAGS += -I../../liblorien
CFLAGS += -I../../libdescriptor

obj += load/load.o
obj += request_writer/request_writer.o
obj += graph/graph.o
obj += ipc/ipc.o
obj += sigutil/sigutil.o
obj += lockfile/lockfile.o
obj += events/events.o
obj += build_slot/build_slot.o
obj += fsent/fsent.o
obj += logging/logging.o
obj += client/client.o
obj += metadata/metadata.o
obj += errtab/FAB.errtab.o
obj += build_slot/build_slot.desc.o
obj += describe/describe.desc.o
obj += list/list.desc.o
obj += metadata/metadata.desc.o
obj += stats/stats.desc.o

libfab.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
