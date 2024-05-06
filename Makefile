CC= gcc
LIBS= -lpthread 
CFLAGS= -rdynamic 
DEBUG= -g
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@ $(LIBS)

main: $(OBJ)
	$(CC) $(DEBUG) $(CFLAGS) $^ -o server $(LIBS)


run: 
	./server
all: main


.PHONY: clean

clean:
	rm  *.o server
