#include<stdio.h>
#include<stdlib.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<signal.h>
#include "poll.h"
#include "debug.h"
#include "request.h"
#define BUFFSIZE 1024

void read_cb (poll_event_t * poll_event, poll_event_element_t * elem, struct epoll_event ev)
{
    // NOTE -> read is also invoked on accept and connect
    INFO("in read_cb");
    // we just read data and print
    char buf[BUFFSIZE];
    int val = read(elem->fd, buf, BUFFSIZE);
    if (val>0)
    {
        // if we actually get data print it
       buf[val] = '\0';
       LOG(" received data -> %s %t\n", buf,clock());
       request *req=request_new(elem->fd,buf);
       request_parse(req);
       request_free(req);
       int sent=write(elem->fd,buf,strlen(buf));
       if(sent==-1)
	       INFO("sent error\n");
       LOG("sent:%d",sent);
    }
}


void close_cb (poll_event_t * poll_event, poll_event_element_t * elem, struct epoll_event ev)
{
    INFO("in close_cb");
    // close the socket, we are done with it
    poll_event_remove(poll_event, elem->fd);
}

void accept_cb(poll_event_t * poll_event, poll_event_element_t * elem, struct epoll_event ev)
{
    INFO("in accept_cb");
    // accept the connection 
    struct sockaddr_in clt_addr;
    socklen_t clt_len = sizeof(clt_addr);
    int listenfd = accept(elem->fd, (struct sockaddr*) &clt_addr, &clt_len);
    fcntl(listenfd, F_SETFL, O_NONBLOCK);
    fprintf(stderr, "got the socket %d\n", listenfd);
    // set flags to check 
    uint32_t flags = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
    poll_event_element_t *p;
    // add file descriptor to poll event
    poll_event_add(poll_event, listenfd, flags, &p);
    // set function callbacks 
    p->read_callback = read_cb;
    p->close_callback = close_cb;
}

//time out function 
int timeout_cb (poll_event_t *poll_event)
{
    // just keep a count
    if (!poll_event->data)
    {
        // no count initialised, then initialize it
        INFO("init timeout counter");
        poll_event->data=calloc(1,sizeof(int));
    }
    else
    {
        // increment and print the count
        int * value = (int*)poll_event->data;
        *value+=1;
        //LOG("time out number %d", *value);
       // printf("tick (%d)\n", *value);
    }
    return 0;
}
int main()
{
    //SIGPIPE handle by kernel	
    struct sigaction sa;
    sa.sa_handler=SIG_IGN;
    sigaction(SIGPIPE,&sa,0);

    // create a TCP socket, bind and listen
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in svr_addr;
    memset(&svr_addr, 0 , sizeof(svr_addr));
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = htons(INADDR_ANY);
    svr_addr.sin_port = htons(8080);
    bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr));
    listen(sock, 10);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    // create a poll event object, with time out of 1 sec
    poll_event_t *pe = poll_event_new(1000);
    // set timeout callback
    pe->timeout_callback = timeout_cb;
    poll_event_element_t *p;
    // add sock to poll event
    poll_event_add(pe, sock, EPOLLIN, &p);
    // set callbacks
    //p->read_callback = read_cb;
    p->accept_callback = accept_cb;
    p->close_callback = close_cb;
    // enable accept callback
    p->cb_flags |= ACCEPT_CB;
    // start the event loop
    use_the_force(pe);

    return 0;
}

