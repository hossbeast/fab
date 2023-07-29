CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common

CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

CFLAGS += -I.
CFLAGS += -Iload
CFLAGS += -Imkdirp
CFLAGS += -Ilorien
CFLAGS += -Inftwat
CFLAGS += -Ipath_normalize
CFLAGS += -Isymlinkp
CFLAGS += -Irmdirp
CFLAGS += -I../libcommon
CFLAGS += -I../include
CFLAGS += -I../libxapi
CFLAGS += -I../libxlinux
CFLAGS += -I../libnarrator
CFLAGS += -I../libvalyria
CFLAGS += -I../liblogger

obj += load/load.o
obj += mkdirp/mkdirp.o
obj += nftwat/nftwat.o
obj += path_normalize/path_normalize.o
obj += symlinkp/symlinkp.o
obj += rmdirp/rmdirp.o

liblorien.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
