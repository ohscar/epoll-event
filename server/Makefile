CC = gcc -fPIC -g
LDFLAGS = -lm

# set DEBUG options
ifdef DEBUG
CFLAGS = -Wall -Wextra -ggdb -pg -DDEBUG 
else
CFLAGS = -Wall -O2 
endif

#name all the object files
OBJS = server.o  poll.o request.o string.o 

all : epoll

epoll : $(OBJS)
	$(CC) $(LDFLAGS) -o server $^

debug :
	make all DEBUG=1

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $^


clean :
	rm -rf $(OBJS) server



