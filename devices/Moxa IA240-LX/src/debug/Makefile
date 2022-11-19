CC=arm-linux-gcc
LIBRARY=../library
CFLAGS=-I$(LIBRARY)
CXXFLAGS=
OBJS1=debug.o socket.o

all:	debug

debug: $(OBJS1)
	$(CC) $(OBJS1) -o $@

socket.o: $(LIBRARY)/socket.c
	$(CC) -c $(LIBRARY)/socket.c

clean:
	rm *.o debug > /dev/null 2>&1
