#include "poll.h"
#include "debug.h"
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include<unistd.h>


 static  poll_event_element_t * _nodes;
//poll_event_element functions
/**
 * Function to allocate a new poll event element
 * @param fd the file descriptor to watch
 * @param events epoll events mask
 * @returns poll event element on success
 * @returns NULL on failure
 */
poll_event_element_t * poll_event_element_new(int fd, uint32_t events)
{
    INFO("Creating a new poll event element");
    poll_event_element_t *elem = calloc(1, poll_event_element_s);
    if (elem)
    {
        elem->fd = fd;
        elem->events = events;
    }
    return elem;
}

/**
 * Function to delete a poll event element
 * @param elem poll event element
 */
void poll_event_element_delete(poll_event_element_t * elem)
{
    INFO("Deleting a poll event element");
    free(elem);
}

// poll_event function
/**
 * Function to create a new poll event object
 * @param timeout timeout for the pollevent
 * @retunrs NULL on failure
 * @retunrs poll event object on sucess
 */
poll_event_t * poll_event_new(int timeout)
{
	_nodes=NULL;
    poll_event_t * poll_event = calloc(1, poll_event_s);
    if (!poll_event)
    {
        INFO("calloc failed at poll_event");
        return NULL; // No Memory
    }
    
    poll_event->timeout = timeout;
    poll_event->epoll_fd = epoll_create(MAX_EVENTS);
    INFO("Created a new poll event");
    return poll_event;
}

/**
 * Function to delete poll event object
 * @param poll_event poll event object to be deleted
 */
void poll_event_delete(poll_event_t* poll_event)
{
    INFO("deleting a poll_event");
    //hash_table_delete(poll_event->table);
    close(poll_event->epoll_fd);
    free(poll_event);
}

/**
 * Function to add a file descriptor to the event poll obeject
 * @note if add is performed on an fd already in poll_event, the flags are updated in the existing object
 * @param poll_event poll event object which fd has to be added
 * @param fd the file descriptor to be added
 * @param flags events flags from epoll
 * @param poll_element a poll event element pointer which is filled in by the function, set all function callbacks and cb_flags in this
 */
int poll_event_add(poll_event_t* poll_event, int fd, uint32_t flags, poll_event_element_t **poll_element)
{
    poll_event_element_t *elem = NULL;
    HASH_FIND_INT(_nodes,&fd,elem);
    if (elem)
    {
        LOG("fd (%d) already added updating flags", fd);
        elem->events |= flags;
        struct epoll_event ev;
        memset(&ev, 0, sizeof(struct epoll_event));
        ev.data.fd = fd;
        ev.events = elem->events;
        *poll_element = elem;
        return epoll_ctl(poll_event->epoll_fd, EPOLL_CTL_MOD, fd, &ev);
    }
    else
    {
        elem = poll_event_element_new(fd, flags);
       
       HASH_ADD_INT(_nodes,fd,elem);
        LOG("Added fd(%d)", fd);
        struct epoll_event ev;
        memset(&ev, 0, sizeof(struct epoll_event));
        ev.data.fd = fd;
        ev.events = elem->events;
        *poll_element = elem;
        return epoll_ctl(poll_event->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    }
}

/**
 * Function to remove a poll event element from the given poll_event object
 * @param poll_event poll event object from which fd has to be removed
 * @param fd file descriptor which has to be removed
 */
int poll_event_remove(poll_event_t* poll_event, int fd)
{
    poll_event_element_t *elem = NULL;
    HASH_FIND_INT(_nodes,&fd,elem);
    if(elem)
    {
	 		HASH_DEL(_nodes,elem);
   	 close(fd);
    	epoll_ctl(poll_event->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    }
    return 0;
}

/**
 * Function which processes the events from epoll_wait and calls the appropriate callbacks
 * @note only process events once if you need to use an event loop use poll_event_loop
 * @param poll_event poll event object to be processed
 */
int poll_event_process(poll_event_t * poll_event)
{
    struct epoll_event events[MAX_EVENTS];
    INFO("May the source be with you!!");
    int fds = epoll_wait(poll_event->epoll_fd, events, MAX_EVENTS, poll_event->timeout);
    if (fds == 0)
    {
        INFO("event loop timed out");
        if (poll_event->timeout_callback)
        {
            if (poll_event->timeout_callback(poll_event))
            {
                return -1;
            }
        }
    }
    int i = 0;
    for(;i<fds;i++)
    {
        poll_event_element_t * value = NULL;
        	HASH_FIND_INT(_nodes,&events[i].data.fd,value);
        if (value)
        {
            LOG("started processing for event id(%d) and sock(%d)", i, events[i].data.fd);
            // when data avaliable for read or urgent flag is set
            if ((events[i].events & EPOLLIN) || (events[i].events & EPOLLPRI))
            {
                if (events[i].events & EPOLLIN)
                {
                    LOG("found EPOLLIN for event id(%d) and sock(%d)", i, events[i].data.fd);
                    value->cur_event &= EPOLLIN;
                }
                else
                {
                    LOG("found EPOLLPRI for event id(%d) and sock(%d)", i, events[i].data.fd);
                    value->cur_event &= EPOLLPRI;
                }
                /// connect or accept callbacks also go through EPOLLIN
                /// accept callback if flag set
                if ((value->cb_flags & ACCEPT_CB) && (value->accept_callback))
                    value->accept_callback(poll_event, value, events[i]);
                /// connect callback if flag set
                if ((value->cb_flags & CONNECT_CB) && (value->connect_callback))
                    value->connect_callback(poll_event, value, events[i]);
                /// read callback in any case
                if (value->read_callback)
                    value->read_callback(poll_event, value, events[i]);
            }
            // when write possible
            if (events[i].events & EPOLLOUT)
            {
                LOG("found EPOLLOUT for event id(%d) and sock(%d)", i, events[i].data.fd);
                value->cur_event &= EPOLLOUT;
                if (value->write_callback)
                    value->write_callback(poll_event, value, events[i]);
            }
            // shutdown or error
            if ( (events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP))
            {
                if (events[i].events & EPOLLRDHUP)
                {
                    LOG("found EPOLLRDHUP for event id(%d) and sock(%d)", i, events[i].data.fd);
                    value->cur_event &= EPOLLRDHUP;
                }
                else
                {
                    LOG("found EPOLLERR for event id(%d) and sock(%d)", i, events[i].data.fd);
                    value->cur_event &= EPOLLERR;
                }
                if (value->close_callback)
                    value->close_callback(poll_event, value, events[i]);
            }
        }
        else // not in table
        {
            LOG("WARNING: NOT FOUND hash table value for event id(%d) and sock(%d)", i, events[i].data.fd);
        }
    } // for
    return 0;
}

/**
 * Function to start the event loop which monitors all fds and callbacks accordingly
 * @note event loop runs indefinitely and can only be stopped by timeout callback, so to process the events only once use poll_event_process
 */
void poll_event_loop(poll_event_t* poll_event)
{
    INFO("Entering the main event loop for epoll lib");
    while(!poll_event_process(poll_event));
} 

