#
# Copyright (c) 1988 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation,
# advertising materials, and other materials related to such
# distribution and use acknowledge that the software was developed
# by the University of California, Berkeley.  The name of the
# University may not be used to endorse or promote products derived
# from this software without specific prior written permission.
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
#	@(#)makefile	4.7 (Berkeley) 6/6/90
#

# Makefile for tn3270 and friends...
#
#	This is the makefile for tn3270.  Note that we use the 4.3+ telnet
# (compiled with special options; see below) to provide the telnet
# support we need.
#
#	The following are the defines that may be passed (via the cc
# -D option) to the compiler.
#
#	TN3270		- 	This is to be linked with tn3270.  Necessary
#				for creating tn3270.  Only for compiling
#				telnet.
#
#	NOT43		-	Allows the program to compile and run on
#				a 4.2BSD system.
#
#	PUTCHAR		-	Within tn3270, on a NOT43 system,
#				allows the use of the 4.3 curses
#				(greater speed updating the screen).
#				You need the 4.3 curses for this to work.
#
#	FD_SETSIZE	-	On whichever system, if this isn't defined,
#				we patch over the FD_SET, etc., macros with
#				some homebrewed ones.
#
#	SO_OOBINLINE	-	This is a socket option which we would like
#				to set to allow TCP urgent data to come
#				to us "inline".  This is NECESSARY for
#				CORRECT operation, and desireable for
#				simpler operation.
#
#	LNOFLSH		-	Detects the presence of the LNOFLSH bit
#				in the tty structure.
#
#
#
#	TERMCAP		-	Define this if your system is termcap based,
#				otherwise a terminfo based system is assumed.
#
#	SRCRT		-	Includes code to allow you to specify
#				source routes.
#				Format is:
#					[!]@hop1@hop2...[@|:]dst
#				Leading ! means strict source route.
#
#	NOSTRNCASECMP	-	Define this if you do not have strncasecmp() in
#				your C libarary.
#
#	USE_TERMIO	-	Define this if you have System V termio
#				structures.  What is here is how things
#				are on Cray computers.
#
#	KLUDGELINEMODE	-	Define this to get the kludged up version
#				of linemode that was in 4.3BSD.  This is a
#				good thing to have around for talking to
#				older systems.
#
#
#	Here are some which are used throughout the system:
#
#	unix		-	Compiles in unix specific stuff.
#
#	msdos		-	Compiles in msdos specific stuff.
#

# msdos versus unix defines
O	= .o
#PC_O	= .obj

X	=
#PC_X	= .exe

L	=
#PC_L	= -link

CC	= cc
#PC_CC	= cl

MV	= mv
#PC_MV	= rename

RM	= rm -f
#PC_RM= erase

LINT_ARGS =
#PC_LINT_ARGS = -DLINT_ARGS

DEBUG_FLAGS = -g
#PC_DEBUG_FLAGS = -Zi -Od

AR	= ar
AR1	= cr
AR2	=
AR3	=
#PC_AR	= lib
#PC_AR1	=
#PC_AR2	= +
#PC_AR3	= ";"

RANLIB	= ranlib
#PC_RANLIB = echo "Done with "


PRINT	= print
ACTION = @sccs tell

DEFINES= ${LINT_ARGS} -DTERMCAP -DSRCRT -DKLUDGELINEMODE -DUSE_TERMIO

INCLUDES = -I. -I..

OPTIMIZE = -O
OPTIMIZE = ${DEBUG_FLAGS}

CFLAGS	= $(OPTIMIZE) $(INCLUDES) $(DEFINES)

# Lint flags
LINTFLAGS	= -hbxaz
# How to install the bloody thing...

DESTDIR=

BINDIR		= $(DESTDIR)/usr/bin
MAN1DIR		= $(DESTDIR)/usr/man/cat1
MAN5DIR		= $(DESTDIR)/usr/man/cat5

# Names for the terminal libraries...
LIBCURSES	= -lcurses
LIBTERM		= -ltermlib

#PC_LIBCURSES	=
#PC_LIBTERM	=

# The source files...
ALLH =	telextrn.h

MSMAIN = ${.CURDIR}/ascii/mset.c

ALLC =

ALLO	= mset$O

ALLHC=	${ALLH} ${ALLC}
ALLPRINT =	${ALLHC}

ALLSOURCE =	${ALLPRINT} makefile makefile.mak makefile_4.2 README \
		mset.1 tn3270.1 map3270.5

SYS	= sys_curses
#PC_SYS	= sys_dos

# The places where the various components live...

SUBDIR =	${.CURDIR}/api ${.CURDIR}/ascii ${.CURDIR}/ctlr \
		${.CURDIR}/general ${.CURDIR}/${SYS} ${.CURDIR}/telnet

# The following are directories we don't do regular make's in, but
# we do make everywhere, print, and sourcelist in.

EXTRADIR =	${.CURDIR}/tools

# The libraries we use.  The order here is important.
# syslib.a and ctlrlib.a should come first, then the rest.
SUBLIB =	${SYS}/syslib.a ctlr/ctlrlib.a \
		ascii/asciilib.a general/generallib.a

.s.o:
	/lib/cpp -E $< | as -o $@

#.c.obj:
#	${CC} ${CFLAGS} -c $<

all:	FRC tn3270$X mset$X

FRC:
	for i in ${SUBDIR}; \
		do (cd $$i; make ${MFLAGS} "CFLAGS=${CFLAGS}"); done

tn3270$X:	telnet/telprog.o ${SUBLIB} api/apilib.a
	${CC} ${CFLAGS} -o tn3270 telnet/telprog.o \
		$L ${SUBLIB} api/apilib.a $(LIBCURSES) $(LIBTERM) \
		-lcompat

#PC_tn3270$X:
#PC_	link <@<
#PC_	telnet
#PC_	tn3270
#PC_	nul
#PC_	${SUBLIB} api/apilib.a+
#PC_	\lib\ublib\ubtcp
#PC_	_PC_<

mset$X:	mset$O ascii/map3270$O
	${CC} ${CFLAGS} -o mset mset$O ascii/map3270$O $L api/apilib.a

mset$O:	$(MSMAIN)
	$(CC) $(CFLAGS) -c $(MSMAIN)

install:	tn3270$X mset$X tn3270.0 mset.0 map3270.0
	install -m 755 -o bin -g bin -s tn3270 $(BINDIR)
	install -m 755 -o bin -g bin -s mset $(BINDIR)
	install -c -o bin -g bin -m 444 tn3270.0 mset.0 $(MAN1DIR)
	install -c -o bin -g bin -m 444 map3270.0 $(MAN5DIR)

action:
	${ACTION}

clist:	${ALLHC}
	@for i in ${SUBDIR}; \
		do (cd $$i; make ${MFLAGS} "DIRPATH=${DIRPATH}$$i/" \
							    clist); done

hclist:	${ALLHC}
	@for i in ${SUBDIR}; \
		do (cd $$i; make ${MFLAGS} "DIRPATH=${DIRPATH}$$i/" \
							    hclist); done

everywhere:	action
	for i in ${SUBDIR} ${EXTRADIR}; \
		do (echo "[$$i]"; cd $$i; make ${MFLAGS} action \
						"ACTION=${ACTION}"); done

cleandir clean:
	for i in $(ALLO) mset tn3270 errs makefile.bak; \
			do (${RM} $$i); done
	for i in ${SUBDIR} ${EXTRADIR}; \
			do (cd $$i; make ${MFLAGS} clean); done

sccsclean:
	-sccs clean
	-sccs get makefile
	for i in ${SUBDIR} ${EXTRADIR}; \
			do (cd $$i; make ${MFLAGS} sccsclean); done

print:
	${PRINT} ${ALLPRINT}
	for i in ${SUBDIR} ${EXTRADIR}; \
			do (cd $$i; make ${MFLAGS} "PRINT=${PRINT}" print); done

tags:	${ALLC} ${ALLH}
	ctags -t `make ${MFLAGS} hclist`

sourcelist:	${ALLSOURCE}
	@for i in ${ALLSOURCE}; \
		do (echo ${DIRPATH}$$i); done
	@for i in ${SUBDIR} ${EXTRADIR}; \
		do (cd $$i; make ${MFLAGS} "DIRPATH=${DIRPATH}$$i/" \
							    sourcelist); done

lint:
	lint ${LINTFLAGS} -Itelnet ${INCLUDES} ${DEFINES} -DTN3270 \
				`make clist` -lcurses

lintmset:
	lint ${LINTFLAGS} ${INCLUDES} ${DEFINES} ${MSMAIN} \
				ascii/map3270.c -lcurses

makefiles.pc:	tools/mkmake
	for i in . ${SUBDIR} ${EXTRADIR}; \
	do (sed -e "s/lib\.a/.lib/g" -e "s/^#PC_//" < $$i/makefile | \
		./tools/mkmake | \
		sed -e "sx/x\\\\xg" -e "s/[ 	]*_PC_//" > $$i/makefile.mak); \
	    done

tools/mkmake:
	(cd tools; make mkmake)

.DEFAULT:
	sccs get $<

depend:	thisdepend
	for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} depend); done

thisdepend:
	echo > eddep.c
	grep '^#include' ${ALLC} eddep.c | grep -v '<' | \
	sed -e 's/:[^"]*"\([^"]*\)".*/: \1/' \
	    -e 's/\.c/$$O/' | \
	awk ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$2 } } \
	      END { print rec } ' > makedep
	echo '$$r makedep' >>eddep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	-rm -f makefile.bak
	cp makefile makefile.bak
	ed - makefile < eddep
	rm eddep makedep eddep.c

# DO NOT DELETE THIS LINE

