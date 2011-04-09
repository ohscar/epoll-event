#include <stdlib.h>
#include "debug.h"
#include "request.h"

request 
*request_new(int client_fd,char *data){

	INFO("create req");
	request *req=(request*)malloc(sizeof(request));
	if(req==NULL){
		INFO("out memory");
		return NULL;
	}
	req->client_fd=client_fd;
	req->data=data;

	return req;
}

void
request_parse(request *req){
//todo something
}

void
request_free(request *req){
	INFO("free req");
	free(req);
}
