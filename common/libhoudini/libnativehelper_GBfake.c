#include <stdio.h>
#include "cutils/log.h"

#define LOG_TAG "libnativehelper_GBfake"

int jniRegisterSystemMethods(void *p) {
    ALOGE("fake jniRegisterSystemMethods() called");
    return 1;
}
