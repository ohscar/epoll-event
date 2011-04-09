typedef enum{NONE=0,GET,PUT} *command;

typedef enum {NO,CMD,LEN,STP} states;
static states _table[256]={NO};

typedef struct{
#ifdef DEBUG 
	unsigned long id;
#endif
	int client_fd;
	char *client_addr;
	char *data;
	char *body;
	int  length;
	command cmd;
}request;

request *request_new(int client_fd,char *data);
void request_parse(request*);
void request_free(request*);
