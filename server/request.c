#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "request.h"
#define MAXLINE 1024



request 
*request_new(int client_fd,char *data){

	INFO("create req");
	request *req=(request*)malloc(sizeof(request));
	if(req==NULL){
		INFO("out memory");
		return NULL;
	}
	req->client_fd=client_fd;
	req->data=strdup(data);

	return req;
}


void
request_parse(request *req)
{

	if(req->data!=NULL)
	{
		unsigned char ch=0;
		char *data=req->data;
		size_t idx=0,pos=0;
		char line[MAXLINE]={0};
		states pre_state=BODY;
		int goon=1;
		while(data[idx]!='\0')
		{
		 if(goon==0)break;

		 ch=(unsigned)data[idx];
	 	 switch(_table[ch])
	 	 {
			case CMD:
				{
					INFO("CMD");
					line[pos++]=data[idx++];
					pre_state=_table[ch];
					break;
				}
			case LEN:
				{
					INFO("LEN");
					line[pos++]=data[idx++];
					pre_state=_table[ch];

					break;
				}
			case STP:
				{
					INFO("STP");
					line[pos]='\0';
					switch(pre_state)
					{
						case CMD:
							req->cmd=strdup(line);
							LOG("cmd:%s",line);
							break;
						case LEN:
							req->length=atoi(line);
							LOG("len:%s",line);
							break;
						default:break;

					}
					line[0]='\0';
					pos=0;
					idx+=2;
					break;
				}
			case BODY:
				{
					INFO("body");
					while(pos<req->length&&data[idx]!='\0'&&pos<MAXLINE)
						line[pos++]=data[idx++];
					line[pos]='\0';
					req->body=strdup(line);
					goon=0;
					break;
				}
			 default:
				goon=0;
				
	 	 }
	 
		}
	}
}

void
request_free(request *req){
	if(!req) return;
	INFO("free req");
	if(req->cmd)
		free(req->cmd);
	if(req->body)
		free(req->body);
	if(req->data)
		free(req->data);
	free(req);
}
