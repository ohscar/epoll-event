
typedef enum {BODY=0,CMD,LEN,STP} states;
static states _table[256]={BODY};

typedef struct{
#ifdef DEBUG 
	unsigned long id;
#endif
	int client_fd;
	int length;
	char *client_addr;
	char *data;
	char *body;
	char *cmd;
}request;

request *request_new(int client_fd,char *data);
void request_parse(request*);
void request_free(request*);
