#	@(#)Makefile	5.6 (Berkeley) 5/11/90

PROG=	enpload
MAN8=	enpload.0

install:
	install ${STRIP} -o ${BINOWN} -g ${BINGRP} -m ${BINMODE} enpload \
	    ${DESTDIR}${BINDIR}
	install -c -o ${MANOWN} -g ${MANGRP} -m ${MANMODE} ${MAN8} \
	    ${DESTDIR}${MANDIR}8/tahoe
	install -c -o ${BINOWN} -g ${BINGRP} -m 444 ${.CURDIR}/enpcode \
	    ${DESTDIR}/etc

.include <bsd.prog.mk>
