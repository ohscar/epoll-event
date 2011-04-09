/* 
*client.c 
*overred
*notice:file's tail(return 0)will cause client to exit
*the server will be getting a SIGPIPE signal while write buffer has unsent stuff
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "common.h"

void connect_flood(int loop)
{
    char *host="127.0.0.1";
    int port=8080;
    struct sockaddr_in servaddr;
    int sockfd;
    
    time_t start=clock();
    int i;
    for(i=0;i<loop;i++)
	{
	    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	    bzero(&servaddr, sizeof(servaddr));
	    servaddr.sin_family = AF_INET;
	    servaddr.sin_addr.s_addr = inet_addr(host);
            servaddr.sin_port = htons(port);

	    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	    close(sockfd);
	}
    time_t end=clock();
    double cost=(end-start)/CLOCKS_PER_SEC;
    printf("connect:%d,cost %lf sec\n",loop,cost);
}

void request_flood(int loop)
{
    char *host="127.0.0.1";
    int port=8080;
    struct sockaddr_in servaddr;
    char buf[MAXLINE]="hello world,i'm overred,ankurs";
    int sockfd;
    
/* ============sockfd============ */
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(host);
    servaddr.sin_port = htons(port);
/* ============connect============ */
    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
/* =========stress testing======== */
    int i=0;
    time_t start=clock();
    for(;i<loop;i++)
    {
    	if(write(sockfd,buf,sizeof(buf))==-1)
		printf("\n** write_1**\n");
    
    }
    time_t end=clock();
    double cost=(double)(end-start)/CLOCKS_PER_SEC;
    printf("request:%d,costed:%lf sec\n",loop,cost);
}

int main(int argc, char *argv[])
{
    if(argc<2)
    {
	printf("pls put loop num\n");
	exit(0);
    }

   request_flood(atoi(argv[1]));
   // connect_flood(atoi(argv[1]));

    return 0;  
}

