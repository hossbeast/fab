CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common

CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

CFLAGS += -I.
CFLAGS += -Imarshal
CFLAGS += -Iunmarshal
CFLAGS += -I../libcommon
CFLAGS += -I../include
CFLAGS += -I../libxapi
CFLAGS += -I../libxlinux

obj += marshal/marshal.o
obj += unmarshal/unmarshal.o

libdescriptor.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
