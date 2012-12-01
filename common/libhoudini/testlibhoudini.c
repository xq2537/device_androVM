#include <stdio.h>
#include <dlfcn.h>
#include "cutils/log.h"

int (*h_init)(int (**f)(int, const char *, const char *, ...)) = NULL;
void * (*h_dlopen)(const char *, int) = NULL;
void * (*h_dlsym)(void *, const char *) = NULL;
void (*h_NativeMethodHelper)(int, void *, int, void *, int, unsigned char *, void **) = NULL;
int (*h_needed)(void *) = NULL;

void * (*h_dvmHoudiniDlopen)(const char *, int) = NULL;
void (*h_dvmHoudiniPlatformInvoke)(void*, void *, int, int, void *, const char*, void*, void *) = NULL;


int main(int argc, char *argv[]) {
    static void *h_handle = NULL;

    if (!dlopen("/system/lib/libnativehelper_GBfake.so", RTLD_NOW|RTLD_GLOBAL))
	fprintf(stderr, "Unable to open libnativehelper_GBfake\n");

    if (h_handle)
        return -1;

    h_handle = dlopen("/system/lib/libdvm_houdini.so", RTLD_NOW);
    if (!h_handle) {
        fprintf(stderr, "Unable to open libdvm_houdini lib: %s\n", dlerror());
        return -1;
    }

    *(void **)(&h_dvmHoudiniDlopen) = dlsym(h_handle, "dvmHoudiniDlopen");
    if (!h_dvmHoudiniDlopen)
        fprintf(stderr, "Unable to find dvmHoudiniDlopen() function\n");

    *(void **)(&h_dvmHoudiniPlatformInvoke) = dlsym(h_handle, "dvmHoudiniPlatformInvoke");
    if (!h_dvmHoudiniPlatformInvoke)
        fprintf(stderr, "Unable to find dvmHoudiniPlatformInvoke() function\n");

    return 0;
}
