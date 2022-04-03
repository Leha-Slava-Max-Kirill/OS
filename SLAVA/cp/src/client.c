#include "funcs.h"

typedef struct args_thrd_
{
    void *socket;
    char login[SIZE];
} Args_thrd_t;

short message_accepted = 0;

void *accept_message(Args_thrd_t *args)
{
    while (1) {
        MessageData_t buf;
        receive_struct(args->socket, &buf);
        if (!strcmp(buf.recipient_login, args->login)) {
            sleep(buf.delay);
            printf("\n<%s> message from %s: %s\n", args->login, buf.sender_login, buf.msg_txt);
            fflush(stdout);
            message_accepted++;
            printf("<%s> ", args->login);
            fflush(stdout);
        }
    }
}

int in(char *login, Logins_t logins)
{
    for (int i = 0; i < logins.count; ++i) {
        if (!strcmp(login, logins.logins[i]))
            return 1;
    }
    return 0;
}

int main()
{
	void *context = zmq_ctx_new();

    void *requester = zmq_socket(context, ZMQ_PUSH);
    char conn_push[] = "tcp://127.0.0.1:5558";
    int rc = zmq_connect(requester, conn_push);
    assert (rc == 0);

    void *subscriber = zmq_socket (context, ZMQ_SUB);
    char conn_sub[] = "tcp://127.0.0.1:5555";
    rc = zmq_connect (subscriber, conn_sub);
    assert (rc == 0);

    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
    assert (rc == 0);

    FILE *fptr_logins;
    fptr_logins = fopen("logins.txt", "r");
    if (fptr_logins == NULL) {
        printf("Error!");   
        exit(1);             
    }

    printf("Hello, mate!\nPlease enter your login: ");
    char login[SIZE];
    scanf("%s", login);

    short i = 0;
    short counter = 1;
    char check_login[SIZE];
    while (1) {
        fscanf(fptr_logins, "%s", check_login);
        if (!strcmp(check_login, login)) {
            printf("Hello, %s!\n", check_login);
            break;           
        }
        if (!strcmp(check_login, "end")) {
            printf("Login %s not found, try again: \n", login);
            if (counter <= 5) {
                scanf("%s", login);
                rewind(fptr_logins);
                counter++;
                continue;
            } else {
                printf("number of tries has exceeded!\n");
                return 0;
            }
        }
        i++;
    }
    fclose(fptr_logins);

    MessageData_t buf;
    printf("started...\n");
    
    pthread_t thrd;
    Args_thrd_t args;
    strcpy(args.login, login);
    args.socket = subscriber;
    pthread_create(&thrd, NULL, accept_message, &args);
    
    while(1)
    {
        int delay;
        printf("<%s> ", login);
        fflush(stdout);
        strcpy(buf.sender_login, login);
        scanf("%s", buf.recipient_login);
        scanf("%s", buf.msg_txt);
        scanf("%d", &buf.delay);
        send_struct(requester, buf);
    }
    pthread_detach(thrd);
    zmq_close(requester);
    zmq_close(subscriber);
    zmq_term(context);
	return 0;
}

