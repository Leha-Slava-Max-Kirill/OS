#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <assert.h>

#define SIZE 256

typedef struct message_
{
    char sender_login[SIZE];
    char recipient_login[SIZE];
    char msg_txt[SIZE];
	int delay;
} MessageData_t;

typedef struct logins_
{
    char logins[SIZE][SIZE];
	short count;
} Logins_t;

void receive_struct(void *socket, MessageData_t *md)
{
	zmq_recv(socket, md->sender_login, sizeof(md->sender_login), 0);
	zmq_recv(socket, md->recipient_login, sizeof(md->recipient_login), 0);
	zmq_recv(socket, md->msg_txt, sizeof(md->msg_txt), 0);
	char delay_str[SIZE];
	zmq_recv(socket, delay_str, sizeof(delay_str), 0);
	sscanf(delay_str, "%d", &md->delay);
}

void send_struct(void *socket, MessageData_t md)
{
	zmq_send(socket, md.sender_login, sizeof(md.sender_login), 0);
	zmq_send(socket, md.recipient_login, sizeof(md.recipient_login), 0);
	zmq_send(socket, md.msg_txt, sizeof(md.msg_txt), 0);
	char delay_str[SIZE];
	sprintf(delay_str, "%d", md.delay);
	zmq_send(socket, delay_str, sizeof(delay_str), 0);
}
