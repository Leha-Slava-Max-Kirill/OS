#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 0x7f
#define ASCENDING 1
#define DESCENDING 0

int activeThreads;
int maxThreads = 6;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int for_test;
typedef struct _stat {
    int* arr;
    int start;
    int count;
    int direction;
} stat;

int* createArray(int n, int* real);
void print(int *arr, int amount);
void BitonicSort(int *arr, int n);
void bitonicSort(int *arr, int start, int count, int direction);
void* bitonicSortThread(void *arg);
void bitonicMerge(int *arr, int start, int count, int direction);

int main(int argc, char **argv) {
    if(argc == 2) {
        maxThreads = atoi(argv[1]);
    }
    int i;
    int n, real;
    printf("Enter amount of numbers\n");
    scanf("%d", &n);
    int* arr = createArray(n, &real);
    for_test = real;
    for (i = 0; i < n; i++) {
        arr[i] = rand() % n; // (N - i);
    }
    printf("n = %d, real = %d\n", n, real);
    // for (int i = 0; i < n; i++) {
    //     scanf("%d", arr + i);
    // }
    printf("Not sorted array\n");
    print(arr, n);
    BitonicSort(arr, real);
    printf("Sorted array\n");
    print(arr, n);
}

int* createArray(int n, int* real) {
    *real = (n > 0) ? 1 : 0;
    while(n > *real) {
        *real *= 2;
    }
    int *arr = malloc(*real * sizeof(int));
    memset(arr, MAX, *real * sizeof(int));
    return arr;
}
void BitonicSort(int *arr, int n) {
    activeThreads = 1;
    bitonicSort(arr, 0, n, ASCENDING);
}
void* bitonicSortThread(void *arg) {
    if(((stat *)arg)->count > 1)
        bitonicSort(((stat *)arg)->arr, ((stat *)arg)->start, ((stat *)arg)->count, ((stat *)arg)->direction);
}
void bitonicSort(int *arr, int start, int count, int direction) {
    if (count < 2) return; 
    int newCount;

    newCount = count / 2;
    pthread_t thread1;
    stat stat1;
    stat1.arr = arr;
    stat1.start = start;
    stat1.count = newCount;
    stat1.direction = ASCENDING;

    pthread_t thread2;
    stat stat2;
    stat2.arr = arr;
    stat2.start = start + newCount;
    stat2.count = newCount;
    stat2.direction = DESCENDING;

        // pthread_mutex_lock(&mutex);
        // activeThreads++;
        // activeThreads++;
        // pthread_mutex_unlock(&mutex);

    pthread_create(&thread1, NULL, bitonicSortThread, &stat1);    
    pthread_create(&thread2, NULL, bitonicSortThread, &stat2);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_mutex_lock(&mutex);
        // activeThreads--;
        // activeThreads--;
    pthread_mutex_unlock(&mutex);
    pthread_mutex_lock(&mutex);
    bitonicMerge(arr, start, count, direction);
    pthread_mutex_unlock(&mutex);
    // } else {
        // pthread_mutex_unlock(&mutex);
        // sleep(1);
        // bitonicSort(arr, start, count, direction);
    // }
    
}
void swap(int* arr, int i, int j, int direction)    
{    
    int tmp;    
    if (direction==(arr[i]>arr[j])) {    
        tmp = arr[i];    
        arr[i] = arr[j];    
        arr[j] = tmp;    
    }    
}    
void bitonicMerge(int *arr, int start, int count, int direction)    
{    
    int newCount, i;    
    if (count > 1) {    
        newCount = count/2;    
        for (i = start; i < start+newCount; i++)    
            swap(arr, i, i+newCount, direction);    
        bitonicMerge(arr, start, newCount, direction);    
        bitonicMerge(arr, start+newCount, newCount, direction);    
    }    
}  
void print(int *arr, int amount) {
    putchar('[');
    for (int i = 0; i < amount; i ++) {
        (i == amount - 1) ? printf("%d", arr[i]) : printf("%d,", arr[i]);
    }
    putchar(']');
    putchar('\n');
}