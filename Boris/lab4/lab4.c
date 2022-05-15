#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>

char* get_string() {
    int len = 0;
    int capacity = 1;
    char *s = (char*) malloc(sizeof(char));
    char c = getchar();
    while (c != '\n') {
        s[(len)++] = c;
        if (len >= capacity) {
            capacity *= 2;
            s = (char*) realloc(s, capacity * sizeof(char));
        }
        c = getchar();
    }
    s[len] = '\0';
    return s;
}
void reverse(char* str, int len) {
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}
int intToStr(int x, char str[], int d)
{
    int i = 0;
    int flag = 0;
    if (x < 0) {
        x *= -1;
        flag = 1;
    }
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
    while ((i < d) || flag == 1) {
        if (flag == 1) {
            str[i++] = '-';
            flag = 0;
        } else str[i++] = '0';
    }
    reverse(str, i);
    str[i] = '\0';
    return i;
}
void ftoa(float n, char* res, int afterpoint) {
    int ipart = (int)n;
    float fpart = n - (float)ipart;
    if (fpart < 0) fpart *= -1;
  
    int i = intToStr(ipart, res, 0);
  
    if (afterpoint != 0) {
        res[i] = '.'; 
        fpart = fpart * pow(10, afterpoint);
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}
int numberOfIntpart(int i) {
    if (i < 0) i *= -1;
    int l = 1;
    while (i / 10) {
        i /= 10;
        l++;
    }
    return l;
}



int main() {
    const char *in_sem_name = "input_semaphor";
    const char *out_sem_name = "output_semaphor";
    sem_unlink(in_sem_name);
    sem_unlink(out_sem_name);
    sem_t *input_semaphore = sem_open(in_sem_name, O_CREAT, 777, 1);
    sem_t *output_semaphore = sem_open(out_sem_name, O_CREAT, 777, 1);
    int pipe1 = open("pipe1.txt", O_RDWR | O_CREAT, 0777);
    int pipe2 = open("pipe2.txt", O_RDWR | O_CREAT, 0777);
    if (pipe1 == -1) {
        printf("ERROR: creating map of file mapping was failed\n");
        exit(EXIT_FAILURE);
    }
    if (pipe2 == -1) {
        printf("ERROR: creating map of file mapping was failed\n");
        exit(EXIT_FAILURE);
    }
    char *pipe_1 = (char*)mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE,  MAP_SHARED, pipe1, 0);
    char *pipe_2 = (char*)mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE,  MAP_SHARED, pipe2, 0);
    if (pipe_1 == MAP_FAILED) {
        printf("ERROR: file1 mappinf was failed\n");
        exit(EXIT_FAILURE);
}
    if (pipe_2 == MAP_FAILED) {
        printf("ERROR: file1 mappinf was failed\n");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid < 0) {
        printf("ERROR: fork was failed\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // child
        sem_wait(input_semaphore);
        struct stat st;
        if(fstat(pipe1, &st)) {
            printf("Error: fstat was failed\n");
            exit(EXIT_FAILURE);
        }
        char* receivedStr = malloc((st.st_size + 1) * sizeof(char));
        *(receivedStr + st.st_size) = '\0';
        for(int i = 0; i < st.st_size; i++) {
            receivedStr[i] = pipe_1[i];
        }
        int file = open(receivedStr, O_RDONLY);
        if (file == -1) {
            printf("ERROR: file '%s' wasn't open. Check the name of file.\n", receivedStr);
            exit(EXIT_FAILURE);
        } else {
            printf("File '%s' was successfully opened.\n", receivedStr);
        }
        free(receivedStr);
        dup2(file, STDIN_FILENO);
        close(file);
        float curr;
        char end;
        float *arr = NULL;
        int counter = 0;
        int numerator = 0;
        int length = 0;
        int writedlength = 0;
        while (scanf("%f", &curr) != EOF) {
            counter++;
            if (counter == 1) {
                numerator = curr;
            }
            if ((arr = realloc(arr, sizeof(curr) * counter - 1)) == NULL) {
                printf("ERROR: realloc was failed\n");
            }
            if (counter > 1) {
                if (curr == 0) {
                    printf("ERROR: division by zero. Stopping program...\n");
                    exit(EXIT_SUCCESS);
                }
                arr[counter - 2] = numerator / curr;
            }
            if (((end = getchar()) == EOF) || (end == '\n')) {
                for(int i = 0; i < counter -1; i++) {
                    char res[20];
                    ftoa(arr[i], res, 4);
                    length += (strlen(res) + 1 )* sizeof(char);
                    // printf("%s ", res);
                }
                // putchar('\n');
                if(ftruncate(pipe2, length)) {
                    printf("ERROR: ftruncate was failed\n");
                    remove("pipe1.txt");
                    remove("pipe2.txt");
                    free(arr);
                    exit(EXIT_FAILURE);
                }
                for (int i = 0; i < counter - 1; i++){
                    char res[20];
                    ftoa(arr[i], res, 4);
                    for (int j = 0; j < strlen(res); j++) {
                        pipe_2[writedlength + j] = res[j];
                    }
                    writedlength += strlen(res) - 1;
                    pipe_2[++writedlength] = ((i == counter - 2) ? '\n' : ' ');
                    writedlength++;
                }
                counter = 0;
                free(arr);
                // printf("length = %d, writedlength = %d\n", length, writedlength);
            }
        }
        sem_post(input_semaphore);
        close(file);
        exit(0);
    } else { // parent
        sem_wait(input_semaphore);
        printf("Parent's id: %d, Child's id: %d\n", getpid(), pid);
        printf("Enter the file's name: ");
        sem_post(output_semaphore);
        int c = 0;
        char* filename = get_string();
        int number = strlen(filename);
        if(ftruncate(pipe1, number)) {
            printf("ERROR: ftruncate was failed\n");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < number; i++) {
            pipe_1[i] = filename[i];
        }
        free(filename);
        sem_post(input_semaphore);
        wait(NULL);
        struct stat st;
        if(fstat(pipe2, &st)) {
            printf("ERROR: fstat of pipe2 was failed\n");
            exit(EXIT_FAILURE);
        }
        // printf("Size of pipe2 = %ld\n", st.st_size);
        printf("\n***Result of division***\n");
        for(int i = 0; i < st.st_size; i++) {
            printf("%c",pipe_2[i]);
        }
        close(pipe1);
        close(pipe2);
        remove("pipe1.txt");
        remove("pipe2.txt");
        sem_destroy(input_semaphore);
        sem_destroy(output_semaphore);
    }
}