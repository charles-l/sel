PREFIX=/usr/local/
CC=cc
INSTALL=install

BIN=sel
OBJ=sel.o
CFLAGS=
LDFLAGS=-lncurses

$(BIN): $(OBJ)
	$(CC) -o sel $(OBJ) $(CFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $<

install: $(BIN)
	$(INSTALL) $(BIN) $(PREFIX)/bin/
	$(INSTALL) sel.1 $(PREFIX)/share/man/man1/

uninstall: $(BIN)
	rm $(PREFIX)/bin/$(BIN)
	rm $(PREFIX)/share/man/man1/sel.1

clean:
	rm $(OBJ) $(BIN)
