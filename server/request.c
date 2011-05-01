#include "request.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string.h"
#include "debug.h"

enum parser_atom{
	 STATE_METHOD,
	 STATE_LENGTH,
	 STATE_BODY,
	 LAST_STATE,
};

typedef int state_fn(struct request *req,struct string  *dest);
state_fn state_method,state_length,state_body,last_state;

static state_fn *States[LAST_STATE+1]={
	[STATE_METHOD]=&state_method,
	[STATE_LENGTH]=&state_length,
	[STATE_BODY]  =&state_body,
	[LAST_STATE]  =&last_state,
};

static int Transforms[LAST_STATE]={
	[STATE_METHOD]=STATE_LENGTH,
	[STATE_LENGTH]=STATE_BODY,
	[STATE_BODY]  =LAST_STATE,
};

enum{
	STATE_CONTINUE,
	STATE_FAIL,
	STATE_DONE,
};

struct request 
*request_new()
{
	struct request *req=(struct request*)malloc(sizeof(struct request));
	return req;
}
int
last_state(struct request *req,struct string *dest)
{
	return STATE_DONE;
}

int
state_method(struct request *req,struct string  *dest)
{
	INFO("in state_method...");
	int term=0;
	char c;
	int i=0;
	int pos=0;
	while(req->data[i]!='\0'){
		c=req->data[i++];
		pos++;
		switch(c){
			case '\r':
				term=1;
				break;
			case '\n':
				if(term){
					req->pos=pos;
					return STATE_CONTINUE;
				}
				else
					return STATE_FAIL;
			default:
				string_putc(dest,c);
				break;
			}
	}
	return STATE_FAIL;
}

int
state_length(struct request *req,struct string  *dest)
{
	INFO("in state_length...");
	int term=0;
	char c;
	int i=req->pos;
	int pos=i;
	LOG("pos:%d",i);
	while(req->data[i]!='\0'){
		c=req->data[i++];
		(pos)++;
		switch(c){
			case '\r':{
				  	term=1;
					break;
				  }
			case '\n':{
				  	if(term){
						req->pos=pos;
						return STATE_CONTINUE;
					}
					else
						return STATE_FAIL;
				  }
			default:
				  if(_table[c]==2)
					  string_putc(dest,c);
				  else
					  return STATE_FAIL;
		}
	}

	return STATE_FAIL;
}

int 
state_body(struct request *req,struct string  *dest)
{
	INFO("in state_body...");
	char c;
	int i=req->pos,c1=0;
	int pos=i;
	LOG("req len:%d",req->length);
	while(req->data[i++]!='\0'&&(c1++)<req->length){
			c=req->data[i];
			string_putc(dest,c);
	}
	return STATE_CONTINUE;
}

void
parse_request(struct request *req)
{
	INFO("in parse_request...");
	int state=STATE_METHOD;
	state_fn *func=States[state];
	struct string *data=string_new(0);
	struct string *dest=data;
	int ret;
	char *tmp;
	while((ret=func(req,dest))!=STATE_DONE){
		if(ret==STATE_FAIL){
			return;
		}
		switch(state){
			case STATE_METHOD:
				tmp=string_detach(dest);
				req->method=strdup(tmp);
				free(tmp);
				break;
			case STATE_LENGTH:
				tmp=string_detach(dest);
				req->length=atoi(tmp);
				free(tmp);
				break;
			case STATE_BODY:
				tmp=string_detach(dest);
				req->body=strdup(tmp);
				free(tmp);
				break;
		}

		state=Transforms[state];
		func=States[state];
	
	}
	string_free(data);
}

void
free_request(struct request *req)
{
	INFO("free request...");
	if(req)
	{
	if(req->method)
		free(req->method);
	if(req->body)
		free(req->body);
	if(req->data)
		free(req->data);
	free(req);
	}
}
