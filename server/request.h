typedef enum{NONE,GET} *command;

typedef struct{
#ifdef DEBUG 
	unsigned long id;
#endif
	int client_fd;
	char *client_addr;
	void* data;
	command cmd;
}request;

request *request_new(int client_fd,char *data);
void request_parse(request*);
void request_free(request*);
