# Makefile for smp (Simple Music Player) - OpenBSD
# ANSI C89 compliant

CC = cc
CFLAGS = -Wall -Wextra -pedantic -ansi -O2
LDFLAGS = -lsndio -lmpg123 -lFLAC -lvorbisfile -lvorbis -logg -lm

# Paths for OpenBSD ports/packages
CFLAGS += -I/usr/local/include
LDFLAGS += -L/usr/local/lib

TARGET = smp
OBJS = smp.o

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/man/man1

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

smp.o: smp.c
	$(CC) $(CFLAGS) -c smp.c

clean:
	rm -f $(TARGET) $(OBJS)

install: $(TARGET)
	mkdir -p $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)
	@echo "Installed to $(BINDIR)/$(TARGET)"

uninstall:
	rm -f $(BINDIR)/$(TARGET)

# OpenBSD package dependencies installation helper
deps:
	@echo "Installing required packages..."
	@echo "Run as root: pkg_add mpg123 flac libvorbis"

.PHONY: all clean install uninstall deps
