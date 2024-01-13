CC= gcc
LIBS= -lpthread `pkg-config --libs gtk+-3.0`
CFLAGS= `pkg-config --cflags gtk+-3.0` -rdynamic
DEBUG= -g
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)
TAGS= . /usr/include/sys /usr/include/netinet /usr/include/gtk-3.0 /usr/include/pango-1.0 /usr/include/glib-2.0 /usr/lib/glib-2.0/include /usr/include/harfbuzz /usr/include/freetype2 /usr/include/libpng16 /usr/include/fribidi /usr/include/cairo /usr/include/pixman-1 /usr/include/gdk-pixbuf-2.0 /usr/include/libmount /usr/include/blkid /usr/include/gio-unix-2.0 /usr/include/atk-1.0 /usr/include/at-spi2-atk/2.0 /usr/include/dbus-1.0 /usr/lib/dbus-1.0/include /usr/include/at-spi-2.

%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@ $(LIBS)

main: $(OBJ)
	$(CC) $(DEBUG) $(CFLAGS) $^ -o server $(LIBS)

tags:
	find $(TAGS) -name "*.[ch]" | etags -  

run: 
	./server
all: main


.PHONY: clean

clean:
	rm  *.o server
