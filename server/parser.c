#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string.h"
#include "debug.h"

enum parser_evt{
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
	STATE_ACCEPT,
	STATE_FAIL,
	STATE_DONE,
};

int
last_state(struct request *req,struct string *dest)
{
	return STATE_DONE;
}

int
state_method(struct request *req,struct string  *dest)
{
	INFO("in state_method...");
	char c;
	int i=0;
	int pos=0;
	while(req->data[i]!='\0'){
		c=req->data[i++];
		pos++;
		if(c=='\r'){
			pos++;//skip \n
			req->pos=pos;
			return STATE_ACCEPT;
		}
		else if(_table[c]==1)
			string_putc(dest,c);
		else{
			req->method=NULL;
			return STATE_FAIL;
		}
	}
	return STATE_FAIL;
}

int
state_length(struct request *req,struct string  *dest)
{
	INFO("in state_length...");
	char c;
	int i=req->pos;
	int pos=i;
	LOG("pos:%d",i);
	while(req->data[i]!='\0'){
		c=req->data[i++];
		(pos)++;
		if(c=='\r'){
			pos++;//skip \n
			req->pos=pos;
			return STATE_ACCEPT;
		}
		else if(c>='0'&&c<='9')
			string_putc(dest,c);
		else
			return STATE_FAIL;
	}
	return STATE_FAIL;
}

int 
state_body(struct request *req,struct string  *dest)
{
	INFO("in state_body...");
	char c;
	int i=req->pos,c1=0;
	size_t body_len=atoi(req->length);
	int pos=i;
	LOG("pos:%d",i);
	while(req->data[i++]!='\0'&&(c1++)<body_len){
			c=req->data[i];
			string_putc(dest,c);
	}
	return STATE_ACCEPT;
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
	while((ret=func(req,dest))!=STATE_DONE){
		if(ret==STATE_FAIL){
			INFO("STATE_FAIL...");
			return;
		}
		switch(state){
			case STATE_METHOD:
				req->method=string_detach(dest);
				LOG("method:%s",req->method);
				break;
			case STATE_LENGTH:
				req->length=string_detach(dest);
				LOG("length:%s",req->length);
				break;
			case STATE_BODY:
				req->body=string_detach(dest);
				LOG("reqbody:%s",req->body);
				break;
		}

		state=Transforms[state];
		func=States[state];
	
	}
}
