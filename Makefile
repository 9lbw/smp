PROG = smp
SRCS = smp.c

SNDFILE_CFLAGS != pkg-config --cflags sndfile
SNDFILE_LIBS   != pkg-config --libs   sndfile

CFLAGS  += -Wall -O2 ${SNDFILE_CFLAGS}
LDADD   += -lsndio ${SNDFILE_LIBS}

# no manual page yet
MAN =

.include <bsd.prog.mk>
