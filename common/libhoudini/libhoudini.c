#include <stdio.h>
#include <dlfcn.h>
#include "cutils/log.h"

#define LOG_TAG "libhoudini_proxy"

int (*h_init)(int (**f)(int, const char *, const char *, ...)) = NULL;
void * (*h_dlopen)(const char *, int) = NULL;
void * (*h_dlsym)(void *, const char *) = NULL;
void (*h_NativeMethodHelper)(int, void *, int, void *, int, unsigned char *, void **) = NULL;
int (*h_needed)(void *) = NULL;

void loadLib() {
    static void *h_handle = NULL;

    if (h_handle)
        return;

    h_handle = dlopen("/system/lib/libhoudini_real.so", RTLD_LAZY);
    if (!h_handle) {
        LOGE("Unable to open libhoudini lib\n");
        return;
    }

    *(void **)(&h_dlopen) = dlsym(h_handle, "dvm2hdDlopen");
    if (!h_dlopen)
        LOGE("Unable to find dvm2hdDlopen() function");

    *(void **)(&h_dlsym) = dlsym(h_handle, "dvm2hdDlsym");
    if (!h_dlsym)
        LOGE("Unable to find dvm2hdDlsym() function");

    *(void **)(&h_init) = dlsym(h_handle, "dvm2hdInit");
    if (!h_init)
        LOGE("Unable to find dvm2hdInit() function");

    *(void **)(&h_NativeMethodHelper) = dlsym(h_handle, "dvm2hdNativeMethodHelper");
    if (!h_NativeMethodHelper)
        LOGE("Unable to find dvm2hdNativeMethodHelper() function");

    *(void **)(&h_needed) = dlsym(h_handle, "dvm2hdNeeded");
    if (!h_needed)
	LOGE("Unable to find dvm2hdNeeded() function");
}

int dvm2hdInit(int (**f)(int, const char *, const char *, ...)) {
	LOGE("Calling dvm2hdInit with f=%p",f);

	loadLib();
	if (h_init) {
		int r;

		r = h_init(f);
		LOGE("will return %d",r);
		return r;
	}
	return 0;
}

void * dvm2hdDlopen(const char *filename, int flags) {
	LOGE("Calling dvm2hdDlopen with filename=%s flags=%d", filename, flags);

	loadLib();
	if (h_dlopen) {
		void *r;

		r = h_dlopen(filename, flags);
		LOGE("will return %p",r);
		return r;
	}
	return NULL;
}

void * dvm2hdDlsym(void *handle, const char *funcname) {
	LOGE("Calling dvm2hdDlsym with handle=%p funcname=%s", handle, funcname);
	
	loadLib();
	if (h_dlsym) {
		void *r;

		r = h_dlsym(handle, funcname);
		LOGE("will return %p",r);
		return r;
	}
	return NULL;
}

void dvm2hdNativeMethodHelper(int p0, void *func, int fsig, void *pReturn, int nArgs, unsigned char *types, void **values) {
	int i;

	LOGE("Calling dvm2hdNativeMethodHelper with func=%p nArgs=%d", func, nArgs);
	for (i=0;i<nArgs;i++) {
		LOGE("    type=%c value=%p", types[i], values[i]);
	}

	loadLib();
	if (h_NativeMethodHelper) {
		h_NativeMethodHelper(p0, func, fsig, pReturn, nArgs, types, values);
	}
}

int dvm2hdNeeded(void *p) {
	LOGE("Calling dvm2hdNeeded with p=%p", p);

	loadLib();
	if (h_needed) {
		int r;

		r = h_needed(p);
		LOGE("will return %d", r);
		return r;
	}
	return 0;
}
