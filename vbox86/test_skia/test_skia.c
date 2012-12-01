#include <dlfcn.h>


int main(int argc, char *argv) {
    void *lib;

    printf("VMCI skia starting\n");

    lib = dlopen("libskia.so", 0);

    printf("dlopen() on libskia.so returned %p\n", lib);

    return 0;
}

