CC=arm-linux-gcc
LIBRARY=../library
CFLAGS=-I$(LIBRARY)
CXXFLAGS=
OBJS1=controller.o socket.o

all:	controller

controller: $(OBJS1)
	$(CC) $(OBJS1) -o $@

socket.o: $(LIBRARY)/socket.c
	$(CC) -c $(LIBRARY)/socket.c

clean:
	rm *.o controller > /dev/null 2>&1

