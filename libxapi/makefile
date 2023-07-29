CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common
CFLAGS += -DXAPI -DXAPI_MODE_STACKTRACE

CFLAGS += -I.
CFLAGS += -Imemblk
CFLAGS += -Iteardown
CFLAGS += -Iinteg
CFLAGS += -Istacktrace
CFLAGS += -Icalltree
CFLAGS += -Iinfo
CFLAGS += -Ihashtable
CFLAGS += -Icolor
CFLAGS += -Itrace
CFLAGS += -Ierrorcode
CFLAGS += -Iexit
CFLAGS += -Imm
CFLAGS += -Iframe
CFLAGS += -Ierrtab
CFLAGS += -I../include

obj += memblk/memblk.def.o
obj += teardown/teardown.o
obj += calltree/calltree.o
obj += info/info.o
obj += hashtable/hashtable.o
obj += color/color.o
obj += trace/trace.o
obj += exit/exit.o
obj += mm/mm.o
obj += frame/frame.o
obj += errtab/errtab.o

libxapi.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
