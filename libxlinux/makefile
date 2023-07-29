CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common

CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

CFLAGS += -I.
CFLAGS += -Isigtable
CFLAGS += -Ixdlfcn
CFLAGS += -Ixdirent
CFLAGS += -Ixfcntl
CFLAGS += -Ixepoll
CFLAGS += -Ixftw
CFLAGS += -Ixfutex
CFLAGS += -Ixinotify
CFLAGS += -Ixgrp
CFLAGS += -Ixfile
CFLAGS += -Ixmman
CFLAGS += -Ixprctl
CFLAGS += -Ixpwd
CFLAGS += -Ixpthread
CFLAGS += -Ixshm
CFLAGS += -Ixsignal
CFLAGS += -Ixresource
CFLAGS += -Ixstdio
CFLAGS += -Ixstring
CFLAGS += -Ixunistd
CFLAGS += -Ixwait
CFLAGS += -Ixtime
CFLAGS += -Ixstdlib
CFLAGS += -Ixstat
CFLAGS += -Ixlinux
CFLAGS += -Imempolicy
CFLAGS += -I../libcommon
CFLAGS += -I../include
CFLAGS += -I../libxapi

obj += load/load.o
obj += sigtable/sigtable.o
obj += xdirent/xdirent.o
obj += xdlfcn/xdlfcn.o
obj += xepoll/xepoll.o
obj += xfcntl/xfcntl.o
obj += xfile/xfile.o
obj += xftw/xftw.o
obj += xfutex/xfutex.o
obj += xgrp/xgrp.o
obj += xinotify/xinotify.o
obj += xmman/xmman.o
obj += xprctl/xprctl.o
obj += xpthread/xpthread.o
obj += xpwd/xpwd.o
obj += xresource/xresource.o
obj += xshm/xshm.o
obj += xsignal/xsignal.o
obj += xstat/xstat.o
obj += xstdio/xstdio.o
obj += xstdlib/xstdlib.o
obj += xstring/xstring.o
obj += xtime/xtime.o
obj += xunistd/xunistd.o
obj += xwait/xwait.o
obj += mempolicy/mempolicy.o
obj += errtab/XLINUX.errtab.o
obj += errtab/KERNEL.errtab.o

libxlinux.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
