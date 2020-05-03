DESTDIR=
PREFIX=/usr

CC 	= gcc
CFLAGS += -g -Wall -D_POSIX_
SRC 	= main.c defines.h

.PHONY: all clean install

all: nibblesd

nibblesd: $(SRC)
	$(CC) $(CFLAGS) -o nibblesd $(SRC) 

clean:
	@rm $(OBJ) nibblesd

install: nibblesd
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 nibblesd $(DESTDIR)$(PREFIX)/bin/nibblesd
