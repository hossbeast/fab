# programs
CC              :=gcc
FLEX						:=flex
BISON						:=bison
SHELL           :=/bin/bash

# paths
SRCDIR           =.
DESTDIR					 =/usr/local/bin
VPATH           +=${SRCDIR}
VPATH						+=$(shell find ${SRCDIR} -mindepth 1 -maxdepth 1 -type d -name '[a-z]*' -printf "%f\n" | grep -v doc)
NAME             =fab

INSTALL         :=install

#
# compiler flags
#  C/L OPTS  - internal to makefile flags
#  C/L FLAGS - user specified compiler flags
# 
COPTS						+=-m64 -g -O0 -Werror -fms-extensions -fno-builtin-log -D_GNU_SOURCE $(foreach x,${VPATH},-I${SRCDIR}/$(x))
%.o : COPTS     +=-c

#
# default goal
#
${NAME}: main.o
	${CC} ${COPTS} ${CFLAGS} ${LOPTS} ${LFLAGS} -o $@ $(foreach x,$(VPATH),$(x)/*.o)

#
# recipes
#

# .y.c's from local or common_c .y's
%.tab.c %.tab.h: %.y
	${BISON} --warnings=error -o $*.tab.c -d ${BISONFLAGS} $^

# .lex.c's from local or common_c .l's
%.lex.c %.lex.h: %.l %.tab.h
	${FLEX} -o $*.lex.c --header-file=$*.lex.h ${FLEXFLAGS} $<

%.tok.c %.tok.h : %.tab.c
	@echo "#ifndef _$(subst .,_,$(*F))_TOK_H"         							 	 >$(*D)/$(*F).tok.h
	@echo "#define _$(subst .,_,$(*F))_TOK_H"         								>>$(*D)/$(*F).tok.h
	@echo "const char* $(subst .,_,$(*F))_tokname(int);"							>>$(*D)/$(*F).tok.h
	@sed   -ne '/yytokentype/,/;/ p' < $<													\
		| sed -e 's/\(yytokentype\)/$(subst .,_,$(*F))_\1/'         \
		| sed -e '/ *[A-Z]/s/\([A-Z]\)/$(subst .,_,$(*F))_\1/'			    >>$(*D)/$(*F).tok.h
	@echo "#endif"                                  									>>$(*D)/$(*F).tok.h
	@echo "typedef unsigned short yytype_uint16;"   									 >$(*D)/$(*F).tok.c
	@sed -ne '/yytname\[\]/,/;/ p'  < $< | sed -e 's/\\"\|'"'"'//g'   >>$(*D)/$(*F).tok.c
	@sed -ne '/yytoknum\[\]/,/;/ p' < $<            									>>$(*D)/$(*F).tok.c
	@grep 'define YYNTOKENS'        < $<            									>>$(*D)/$(*F).tok.c
	@echo '#define API __attribute__((visibility("protected")))'			>>$(*D)/$(*F).tok.c
	@echo '#include <stdio.h>'																				>>$(*D)/$(*F).tok.c
	@echo "const char API * $(subst .,_,$(*F))_tokname(int tok)"			>>$(*D)/$(*F).tok.c
	@echo "{"																													>>$(*D)/$(*F).tok.c
	@echo '  int x;'																									>>$(*D)/$(*F).tok.c
	@echo '  for(x = 0; x < YYNTOKENS; x++)'													>>$(*D)/$(*F).tok.c
	@echo '  {'																												>>$(*D)/$(*F).tok.c
	@echo '  	 if(yytoknum[x] == tok)'																>>$(*D)/$(*F).tok.c
	@echo '		 {'																											>>$(*D)/$(*F).tok.c
	@echo '      return yytname[x];'																	>>$(*D)/$(*F).tok.c
	@echo '    }'																											>>$(*D)/$(*F).tok.c
	@echo '  }'																												>>$(*D)/$(*F).tok.c
	@echo '  return 0;'																								>>$(*D)/$(*F).tok.c
	@echo '}'																													>>$(*D)/$(*F).tok.c


bf/bf.tokens.c : bf/bf.tok.c
	cp $< $@

bf/bf.tokens.h : bf/bf.tok.h
	cp $< $@

%.c : %.y
%.c : %.l

%.o : %.c
	$(CC) $(COPTS) $(CFLAGS) $< -o $@

# dependencies
main.o  : args/args.o						\
				  bf/bf.o								\
				  bp/bp.o								\
				  common/coll.o					\
				  common/idx.o					\
				  common/pstring.o			\
					common/rpath.o				\
				  common/unitstring.o		\
				  common/xmem.o					\
				  common/xstring.o			\
				  fml/fml.o							\
				  gn/gn.o								\
				  log/log.o							\
				  tmp/tmp.o							\
				  ts/ts.o

common/idx.o : common/xstring.o

bf/bf.o : bf/bf.tokens.h bf/bf.tab.o bf/bf.lex.o bf/bf.tokens.o

gn/gn.o	: common/coll.o common/idx.o common/unitstring.o

#
# phony targets
#

.PHONY: install uninstall clean

install: ${NAME}
	${INSTALL} -d               ${DESTDIR}
	${INSTALL} ${NAME}  ${DESTDIR}/${NAME}

uninstall:
	rm -f												${DESTDIR}/${NAME}

clean ::
	rm ${NAME} 1>/dev/null 2>&1 ; true
	find . \( -false $(foreach txt,*.o *.i *.s *.so *.lex.* *.tab.* *.tok.* *.tokens.* exports,-o -name '$(txt)') \) -delete
