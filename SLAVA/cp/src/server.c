#include "funcs.h"

int main()
{
    printf("Enter logins: \n");
    char logins[SIZE][SIZE];
    FILE *fptr_logins;
    fptr_logins = fopen("logins.txt", "w");
    if (fptr_logins == NULL) {
        printf("Error!");   
        exit(1);             
    }
    int i = 0;
    while (1) {
        scanf("%s", logins[i]);
        if (!strcmp(logins[i], "end"))
            break;
        i++;
    }
    const short number_of_users = i;
    printf("Users:\n");
    for (int i = 0; i < number_of_users; ++i) {
        fprintf(fptr_logins, "%s\n", logins[i]);
    }
    fprintf(fptr_logins, "%s\n", "end");
    fclose(fptr_logins);

	void *context = zmq_ctx_new();

    void *requester = zmq_socket(context, ZMQ_PULL);
    char conn_pull[] = "tcp://127.0.0.1:5558";
    int rc = zmq_bind(requester, conn_pull);
    assert (rc == 0);

    void *publisher = zmq_socket(context, ZMQ_PUB);
    char conn_pub[] = "tcp://127.0.0.1:5555";
    rc = zmq_bind(publisher, conn_pub);
    assert (rc == 0);

    MessageData_t buf;
    printf("started...\n");
    while(1)
    {
    	receive_struct(requester, &buf);
        send_struct(publisher, buf);
    }
    zmq_close(requester);
    zmq_term(context);
    remove("logins.txt");
	return 0;
}

