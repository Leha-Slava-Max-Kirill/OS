// to run: gcc main.c -o main -lpthread -lrt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#define SEM "/semaphore"
#define SIZE 200

int count(char *str)
{
    int counter = 1;
    for (int i = 0; i < strlen(str); ++i)
        if (str[i] == ' ')
            counter++;
    return counter;
}

float sum(FILE *f, int count)
{
    float res = 0.0, a;
    rewind(f);
    for (int i = 0; i < count; ++i) {
        fscanf(f, "%f", &a);
        res += a;
    }
    return res;
}

int main(int argc, char const *argv[])
{
    // remove semaphore if it was not deleted
    sem_unlink(SEM);
    // create semaphore with:
    // name SEM, flag O_CREAT|O_EXCL means
    // that an error is returned if a semaphore with the given name already exists
    // create file with READ and WRITE permissions only for me
    // initial value of the semaphore will be 1 (busy)
    sem_t* semaphore  = sem_open(SEM, O_CREAT|O_EXCL, 0600, 1);
    if (semaphore == SEM_FAILED) {
        perror("semaphore");
        exit(1);
    }

    char file_name[SIZE];
    char read_file_name[SIZE];
    char sequence_of_numbers[SIZE];
    char read_sequence_of_numbers[SIZE];

    // create file with READ and WRITE permissions only for me
    int shared_fds1 = open("shared_fds1.txt",  O_RDWR | O_CREAT, 0600);
    if (shared_fds1 == -1) {
        perror("create file");
        exit(1);
    }

    // 0 - we dont care were our memory will be stored
    // page size, read and write permissions
    // file descriptor is our shared_fds1
    // shift is 0
    char* shared_fds_1 = (char*)mmap(0, sysconf(_SC_PAGESIZE), PROT_READ | PROT_WRITE,  MAP_SHARED, shared_fds1, 0);
    if (shared_fds_1 == MAP_FAILED){
        perror("map");
        exit(1);
    }
    printf("Print input name of file: ");
    fgets(file_name, SIZE, stdin);
    if (file_name[strlen(file_name) - 1] == '\n')
        file_name[strlen(file_name) - 1] = '\0';
    
    // only writing
    int f = open(file_name, O_WRONLY | O_CREAT, 0600);
    if (f == -1) {
        perror("create file");
        exit(1);
    }

    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(1);
    }
    // Child Process
    else if (child_pid == 0) {
        // decrement semaphore to 0
        sem_wait(semaphore);
        printf("[Child Process, id=%d]: computing...\n", getpid());
        // f (FD) assign to 1 (stdout)
        if (dup2(f, 1) == -1) {
            perror("dup2");
            exit(1);
        }
        for (int i = 0; i < strlen(shared_fds_1); i++) {
            read_sequence_of_numbers[i] = shared_fds_1[i];
        }
        int cnt = count(read_sequence_of_numbers);
        //
        FILE *write_seq;
        if ((write_seq = fopen(file_name, "w")) == NULL) {
            perror("open file");
            exit(1);
        } // added error handling
        fputs(read_sequence_of_numbers, write_seq);
        fclose(write_seq);

        FILE *read_seq;
        if ((read_seq = fopen(file_name, "r")) == NULL) {
            perror("open file");
            exit(1);
        } // added error handling
        float res = sum(read_seq, cnt);
        fclose(read_seq);

        if (ftruncate(f, sizeof(res))) {
            perror("ftruncate");
            exit(1);    
        }
        printf("%f", res);
        // increment semaphore to 1
        sem_post(semaphore);
    }
    // Parent process 
    else {
        // decrement semaphore to 0
        sem_wait(semaphore);
        printf("[%d] It's parent. Child id: %d\n", getpid(), child_pid);
        // write to memory 
        printf("[Parent Process, id=%d]: Enter the sequence of real type numbers: ", getpid());   
        fgets(sequence_of_numbers, SIZE, stdin);
    
        int length = strlen(sequence_of_numbers) * sizeof(char);
        // truncate shared mamory to the sequence_of_numbers size (in bytes)
        if (ftruncate(shared_fds1, length)) {
            perror("ftruncate");
            exit(1);    
        }
        for(int i = 0; i < strlen(sequence_of_numbers); i++){
            shared_fds_1[i] = sequence_of_numbers[i];
        }
        printf("[Parent Process, id=%d]: Writing to the shared_fds. Message is sequence of numbers: %s\n\n", getpid(), sequence_of_numbers);
        
        // increment semaphore to 1
        sem_post(semaphore);
        close(shared_fds1);
        close(f);
        sem_close(semaphore);
        remove("shared_fds1.txt");
        return 0;   
    }
}