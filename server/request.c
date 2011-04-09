#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "request.h"
#define MAXLINE 1024*10



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
request_parse(request *req)
{

	_table['G']=CMD;
	_table['E']=CMD;
	_table['T']=CMD;
	int ct;
	for(ct=48;ct<58;ct++)
		_table[ct]=LEN;
	_table['\r']=STP;
	_table['\n']=STP;
	
	if(req->data!=NULL)
	{
		char ch=req->data[0];
		int len=strlen(req->data);
		char *data=req->data;
		int idx=0,pos=0;
		char buf[1024]={0};
		states pre_state=NO;
		int goon=1;
		while(data[idx]!='\0'&&goon==1)
		{
	  	 char ch=data[idx];
	 	 switch(_table[ch])
	 	 {
			case CMD:
				{
					INFO("CMD");
					buf[pos++]=data[idx++];
					pre_state=_table[ch];
					break;
				}
			case LEN:
				{
					INFO("LEN");
					buf[pos++]=data[idx++];
					pre_state=_table[ch];

					break;
				}
			case STP:
				{
					INFO("STP");
					buf[pos]='\0';
					switch(pre_state)
					{
						case CMD:
							req->cmd=buf;
							LOG("cmd:%s",buf);
							break;
						case LEN:
							req->length=atoi(buf);
							LOG("len:%s",buf);
							break;

					}
					buf[0]='\0';
					pos=0;
					idx+=2;
					break;
				}
			case NO:
				{
					while(pos<req->length&&data[idx]!='\0')
						buf[pos++]=data[idx++];
					buf[pos]='\0';
					req->body=buf;
					break;
				}
			 default:
				INFO("DEFAILT");
				goon=0;
				
	 	 }
	 
		}
	}
}

void
request_free(request *req){
	INFO("free req");
	free(req);
}
