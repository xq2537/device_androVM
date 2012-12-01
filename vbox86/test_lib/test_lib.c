#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int argc, char *argv[]) {
    int a;

    if (argc<2) {
        fprintf(stderr, "Usage: %s lib\n", argv[0]);
        exit(1);
    }

    for (a=1;a<argc;a++) {
        void *handle = dlopen(argv[a], RTLD_LAZY|RTLD_GLOBAL);
        printf("dlopen on lib %s with RTLD_LAZY returns %p\n", argv[1], handle);
        if (!handle)
            fprintf(stderr, "%s\n", dlerror());
    }

    return 0;
}
