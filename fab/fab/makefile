CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common
CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

# builtin rules are fucking stupid
.SUFFIXES:

CFLAGS += -I.
CFLAGS += -Icommand
CFLAGS += -Iargs
CFLAGS += -Icommand/kill
CFLAGS += -Icommand/autobuild
CFLAGS += -Icommand/touch
CFLAGS += -Icommand/stats
CFLAGS += -Icommand/build
CFLAGS += -Icommand/config
CFLAGS += -Icommand/adhoc
CFLAGS += -Icommand/events
CFLAGS += -Icommand/tree
CFLAGS += -Icommand/ls
CFLAGS += -Icommand/vars
CFLAGS += -Icommand/reconcile
CFLAGS += -Icommand/metadata
CFLAGS += -Icommand/describe
CFLAGS += -Ilogging
CFLAGS += -Ierrtab
CFLAGS += -Iparams
CFLAGS += -I../../libcommon
CFLAGS += -I../../include
CFLAGS += -I../../libxapi
CFLAGS += -I../../libxlinux
CFLAGS += -I../../libnarrator
CFLAGS += -I../../libvalyria
CFLAGS += -I../../liblogger
CFLAGS += -I../../libdescriptor
CFLAGS += -I../../libyyutil
CFLAGS += -I../libfab

LFLAGS += -Wl,--start-group
LFLAGS += ../../libcommon/libcommon.final.a
LFLAGS += ../../libxapi/libxapi.final.a
LFLAGS += ../../libnarrator/libnarrator.final.a
LFLAGS += ../../libdescriptor/libdescriptor.final.a
LFLAGS += ../libfab/libfab.final.a
LFLAGS += ../../libxlinux/libxlinux.final.a
LFLAGS += ../../liblogger/liblogger.final.a
LFLAGS += ../../libvalyria/libvalyria.final.a
LFLAGS += ../../liblorien/liblorien.final.a
LFLAGS += ../../libyyutil/libyyutil.final.a
LFLAGS += -Wl,--end-group

obj += main.o
obj += command/kill/kill.o
obj += command/autobuild/autobuild.o
obj += command/touch/touch.o
obj += command/stats/stats.o
obj += command/build/build.o
obj += command/config/config.o
obj += command/adhoc/adhoc.o
obj += command/events/events.o
obj += command/tree/tree.o
obj += command/ls/ls.o
obj += command/vars/vars.o
obj += command/reconcile/reconcile.o
obj += command/metadata/metadata.o
obj += command/describe/describe.o
obj += logging/logging.o
obj += errtab/MAIN.errtab.o
obj += errtab/CONFIG.errtab.o
obj += params/params.o
obj += args/args.o
obj += args/args.lex.o
obj += args/args.states.o
obj += args/args.tab.o
obj += args/args.tokens.o

fab.final.a : $(obj)
	cc -o foo $(CFLAGS) $^ $(LFLAGS) && mv foo $@

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
