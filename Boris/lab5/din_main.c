#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef enum {
    FIRST,
    SECOND,
} MODE;

MODE r = FIRST;
const char* libName1 = "lib1.so";
const char* libName2 = "lib2.so";

double (*ComputePi)(int k) = NULL;
double (*ComputeE)(int x) = NULL;
char *err;

void *libHandle = NULL;

void loadDLibs(MODE context){
    const char *name;
    name = (context == FIRST) ? libName1 : libName2;
    libHandle = dlopen(name, RTLD_LAZY); //RTLD_LAZY подразумевает разрешение неопределенных символов в виде кода, содержащегося в исполняемой динамической библиотеке
    if(!libHandle){
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

void unloadDLibs(){
    dlclose(libHandle);
}

void loadContext(){
    loadDLibs(r);
    ComputePi = dlsym(libHandle, "ComputePi");
    ComputeE = dlsym(libHandle, "ComputeE");
    if((err = dlerror())) {
        fprintf(stderr, "%s\n", err);
        exit(EXIT_FAILURE);
    }
}

void changeContext(){
    unloadDLibs();
    if(r == FIRST){
        r = SECOND;
    } else {
        r = FIRST;
    }
    loadContext();
}

void print(MODE context, int counter) {
    (counter > 0) ? printf("***************************\n") : counter;
    (counter > 0) ? printf("Press Enter to countinue...\n") : counter;
    (counter > 0) ? getchar() : counter;
    (counter > 0) ? getchar() : counter;
    system("clear");
    printf("+-------------------------------------Mode:  %d-------------------------------------+\n", context + 1);
    switch (context)
    {
    case FIRST :
        printf("|Enter -- 1. To calculate the number pi with Leibniz Series with the long series k |\n");
        printf("|Enter -- 2. To calculate the number e with Teylor series with the long series k   |\n");
        break;
    case SECOND :
        printf("|Enter -- 1. To calculate the number pi with  Wallis formula with the long series k|\n");
        printf("|Enter -- 2. To calculate the number e with limit (1 + 1 / x) ^ x.                 |\n");
        break;
    }
    printf("+----------------------------------------------------------------------------------+\n");
    printf("|Enter -- 0. If you want to change methods(Mode) of calculation.                   |\n");
    printf("|__________________________________________________________________________________|\n");
}

int main(){
    r = FIRST;
    loadContext();
    int task = 0;
    print(r, 0);
    while (scanf("%d", &task) != EOF){
        if(task == 0){
            changeContext();
            print(r, 0);
            puts("Ok. Mode was changed");
            if(r == FIRST){
                puts("The first mode is now enabled");
            } else{
                puts("The second mode is now enabled");
            }
            print(r, 1);
            continue;
        } else if(task == 1) {
            int k;
            printf("\nEnter the series lenght k: ");
            scanf("%d", &k);
            printf("The value of the counted number pi = %.9lf\n", ComputePi(k));    
        } else if(task == 2) {
            //Сделать перевод строк(перепутал вариант)
            int x;
            printf("\nEnter the number: ");
            scanf("%d", &x);
            printf("The value of the counted number e: %lf\n", ComputeE(x));

        } else  printf("This command is not supported, enter 1, 2 or 0\n");      
        print(r, 1);
    }
    system("clear");
    unloadDLibs();
}