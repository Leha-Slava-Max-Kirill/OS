#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

float (*SinIntegral)(float, float, float) = NULL;
float (*Square)(float, float) = NULL;

char *Lib1 = "./lib1.so"; 
char *Lib2 = "./lib2.so";

char *lib_name;
void *current_handle;

void load_functions(void* handle)
{
    dlerror();
    // assign pointer to place in
    // memory where function is stored 
    SinIntegral = dlsym(handle, "SinIntegral");
    dlerror();
    Square = dlsym(handle, "Square");
    dlerror();
}

void *load()
{
    // load dynamic lib
    // RTLD_LAZY, подразумевающим разрешение
    // неопределенных символов в виде кода,
    // содержащегося в исполняемой динамической библиотеке
    void *handle = dlopen(Lib1, RTLD_LAZY);
    if (handle == NULL) {
        printf("Open dl error\n");
        exit(1);
    }
    load_functions(handle);
    return handle;
}

int main()
{
    current_handle = load();
    lib_name = Lib2;
    char mode;
    printf("To compute integral sin(x) [A, B] with accuracy = e enter -- 1 A B e\nTo compute figure area with sides lengths A, B enter -- 2 A B\n");
    while ((mode = getchar()) != EOF) {
        if (mode == '\n') continue;
        if (mode == '1') {
            float A, B, e;
            scanf("%f%f%f", &A, &B, &e);
            printf("Integral: %.10f\n", SinIntegral(A, B, e));
        } else if (mode == '2') {
            float A, B;
            scanf("%f%f", &A, &B);
            printf("Area: %f\n", Square(A, B));
        } else {
            printf("This command is not supported, enter 1 or 0\n");
        }
    }
    dlclose(current_handle);
    return 0;
}