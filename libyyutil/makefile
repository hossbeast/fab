CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common

CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

CFLAGS += -I.
CFLAGS += -Iload
CFLAGS += -Iyyutil
CFLAGS += -Iload
CFLAGS += -Ibox
CFLAGS += -Iparser
CFLAGS += -Igrammar
CFLAGS += -Iscanner
CFLAGS += -Ilogging
CFLAGS += -I../libcommon
CFLAGS += -I../include
CFLAGS += -I../libxapi
CFLAGS += -I../libxlinux
CFLAGS += -I../libnarrator
CFLAGS += -I../libvalyria
CFLAGS += -I../liblogger

obj += load/load.o
obj += box/box.o
obj += parser/parser.o
obj += grammar/grammar.o
obj += scanner/scanner.o
obj += logging/logging.o

libyyutil.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
