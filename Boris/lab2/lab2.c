#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

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
void division(float numerator, float denominator, float *res) {
    *res = numerator / denominator;
}
int main() {
    int fd[2];
    int fd2[2];
    if(pipe(fd)) {
        printf("ERROR: pipe was failed\n");
        exit(EXIT_FAILURE);
    }
    if(pipe(fd2)) {
        printf("ERROR: pipe was failed\n");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid < 0) {
        printf("ERROR: fork was failed\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // child
        close(fd[1]);
        close(fd2[0]);
        printf("Child's id %d\n", getpid());
        int strSize;
        read(fd[0], &strSize, sizeof(strSize));
        char* receivedStr = malloc((strSize + 1) * sizeof(char));
        *(receivedStr + strSize) = '\0';
        read(fd[0], receivedStr, sizeof(char) * strSize);
        close(fd[0]);
        int file = open(receivedStr, O_RDONLY);
        if (file == -1) {
            printf("ERROR: file wasn't open. Check the name of file.\n");
            exit(EXIT_FAILURE);
        }
        printf("File(%s) was successfully opened.\n", receivedStr);
        free(receivedStr);
        dup2(file, STDIN_FILENO);
        close(file);
        char str[20] = {'\0'};
        dup2(fd2[1], STDOUT_FILENO);
        close(fd2[1]);
        float numerator, denominator1, denominator2, res;
        while (scanf("%f%f%f", &numerator, &denominator1, &denominator2) != EOF) {
            if ((denominator1 == 0) || (denominator2 == 0)) exit(EXIT_FAILURE);
            division(numerator, denominator1, &res);
            write(STDOUT_FILENO, &res, sizeof(res));
            division(numerator, denominator2, &res);
            write(STDOUT_FILENO, &res, sizeof(res));
        }
        close(STDOUT_FILENO);
        exit(0);
    } else { // parent
        close(fd[0]);
        close(fd2[1]);
        printf("Parent's id: %d, Child's id: %d\n", getpid(), pid);
        printf("Enter the file's name: ");
        int c = 0;
        char* filename = get_string();
        int number = strlen(filename);
        if (write(fd[1], &number, sizeof(number)) == -1) {
            printf("ERROR: cannot write to pipe\n");
            exit(EXIT_FAILURE);
        }
        if (write(fd[1], filename, sizeof(char) * number) == -1) {
            printf("ERROR: cannot write to pipe\n");
            exit(EXIT_FAILURE);
        }
        close(fd[1]);
        free(filename);
        int counter = 1;
        float res1, res2;
        while(1) {
            if (read(fd2[0], &res1, sizeof(res1)) == 0) break;
            read(fd2[0], &res2, sizeof(res2));
            printf("%d: The result of devision: first %f, second %f\n", counter, res1, res2);
            counter++;
        }
        close(fd2[0]);
    }
}