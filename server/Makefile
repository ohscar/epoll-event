CC = gcc -fPIC 
LDFLAGS = -lm

# set DEBUG options
ifdef DEBUG
CFLAGS = -Wall -Wextra -ggdb -pg -DDEBUG 
else
CFLAGS = -Wall -O2 
endif

#name all the object files
OBJS = server.o hash_table/hashtable.o poll.o request.o

all : epoll hash_table

epoll : $(OBJS)
	$(CC) $(LDFLAGS) -o server $^

hash_table : 
	make -C hash_table

debug :
	make all DEBUG=1

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $^


clean :
	rm -rf $(OBJS) server



