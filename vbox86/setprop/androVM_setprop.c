#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cutils/properties.h>
#include <cutils/log.h>

#define LOG_TAG "androVM_setprop"

int main(int argc, char *argv[]) {
    if (argc != 3) 
        return -1;

    if (strncmp(argv[1], "androVM", 7) != 0) {
        ALOGE("Trying to set non-androVM prop : %s", argv[1]);
        return -1;
    }

    ALOGE("Setting %s to %s", argv[1], argv[2]);

    return property_set(argv[1], argv[2]);
}
