CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common
CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

CFLAGS += -I.
CFLAGS += -Igrowing
CFLAGS += -Iload
CFLAGS += -Ivtable
CFLAGS += -Irecord
CFLAGS += -Iunits
CFLAGS += -Inarrator
CFLAGS += -Ifixed
CFLAGS += -Iinteg
CFLAGS += -Inullity
CFLAGS += -Imulti
CFLAGS += -Ibase
CFLAGS += -Ifd
CFLAGS += -Irolling
CFLAGS += -I../libcommon
CFLAGS += -I../include
CFLAGS += -I../libxapi
CFLAGS += -I../libxlinux
CFLAGS += -I../libnarrator
CFLAGS += -I../libvalyria
CFLAGS += -I../liblogger

obj += growing/growing.o
obj += load/load.o
obj += record/record.o
obj += units/units.o
obj += fixed/fixed.o
obj += nullity/nullity.o
obj += multi/multi.o
obj += base/base.o
obj += fd/fd.o
obj += rolling/rolling.o

libnarrator.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
