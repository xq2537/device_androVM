/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <strings.h>

#include <poll.h>
#include <pthread.h>

#include <linux/input.h>

#include <cutils/atomic.h>
#include <cutils/log.h>
#include <cutils/properties.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "nusensors.h"

struct vsensor_accel_data {
    unsigned char id;
    unsigned char res1;
    char val_x[16];
    char val_y[16];
    char val_z[16];
};


static int poll__close(struct hw_device_t *dev)
{
    if (dev)
        free(dev);
    return 0;
}

static int poll__activate(struct sensors_poll_device_t *dev,
        int handle, int enabled) {
    ALOGE("poll__activate() with handle=%d enabled=%d", handle, enabled);
    return 0;
}

static int poll__setDelay(struct sensors_poll_device_t *dev,
        int handle, int64_t ns) {
    ALOGE("poll__setDelay() with handle=%d ns=%ld", handle, ns);
    return 0;
}

static int64_t getTimestamp() {
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec)*1000000000LL + t.tv_nsec;
}

static int poll__poll(struct sensors_poll_device_t *dev,
        sensors_event_t* data, int count) {
    static int csocket = -1;
    int n = 0;

    // ALOGE("poll() called for %d events", count);

    // If needed, connect to sensor server on host

socket_connect:
    while (csocket < 0) {
        char androVM_server_prop[PROPERTY_VALUE_MAX];
        struct sockaddr_in srv_addr;
        long haddr;

        for (;;) {
            property_get("androVM.server.ip", androVM_server_prop, "");
            if (strlen(androVM_server_prop)>0)
                break;
            sleep(1);
        }
        bzero(&srv_addr, sizeof(srv_addr));
        srv_addr.sin_port = htons(22470);
        srv_addr.sin_family = AF_INET;
        haddr = inet_addr(androVM_server_prop);
        if (haddr == -1) {
            ALOGE("Unable to resolve addr");
            return -1;
        }
        bcopy(&haddr, &srv_addr.sin_addr, sizeof(haddr));

        if ((csocket = socket(AF_INET, SOCK_STREAM, 0))<0) {
            ALOGE("Unable to create socket");
            return -1;
        }

        if (connect(csocket, (struct sockaddr *)&srv_addr, sizeof(srv_addr))<0) {
            ALOGE("Unable to connect to Sensors server...");
            close(csocket);
            csocket = -1;
            sleep(10);
            continue;
        }

        ALOGE("Connection OK to Sensors server");
    }

    // Get the data
    while (1) {
        struct vsensor_accel_data adata[count];

        int sread = read(csocket, adata, sizeof(vsensor_accel_data)*count);
        if (sread < 0) {
            ALOGE("Error reading datas, errno=%d", errno);
            close(csocket);
            csocket = -1;
            goto socket_connect;
        }
        if (sread == 0) {
            ALOGE("connection closed, reconnecting...");
            close(csocket);
            csocket = -1;
            goto socket_connect;
        }
        if ((sread % sizeof(vsensor_accel_data)) != 0) {
            ALOGE("read() returned %d bytes, not a multiple of %d !", sread, sizeof(vsensor_accel_data));
            continue;
        }

        int nbEvents = sread/sizeof(vsensor_accel_data);
        //ALOGE("%d accel events received", nbEvents);
        for (int i=0; i<nbEvents; i++) {
            if (adata[i].id != SENSOR_TYPE_ACCELEROMETER) {
                ALOGE("Unknown sensor type : %d !", adata[i].id);
                continue;
            }
             
            data[i].version = sizeof(sensors_event_t);
            data[i].sensor = 0;
            data[i].type = SENSOR_TYPE_ACCELEROMETER;
            data[i].timestamp = getTimestamp();
            memset(data[i].data, 0, sizeof(data[i].data));
            sscanf(adata[i].val_x, "%f", &(data[i].acceleration.x));
            sscanf(adata[i].val_y, "%f", &(data[i].acceleration.y));
            sscanf(adata[i].val_z, "%f", &(data[i].acceleration.z));
            //ALOGE("Acceleration data: %f %f %f", data[i].acceleration.x, data[i].acceleration.y, data[i].acceleration.z);
        }

        return nbEvents;
    }
}

/*****************************************************************************/

int init_nusensors(hw_module_t const* module, hw_device_t** device)
{
    struct sensors_poll_device_t *dev;

    dev = (struct sensors_poll_device_t *)malloc(sizeof(struct sensors_poll_device_t));
    if (!dev) {
        ALOGE("Unable to allocate sensors_poll_device_t");
        return -1;
    }

    memset(dev, 0, sizeof(sensors_poll_device_t));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version  = 0;
    dev->common.module   = const_cast<hw_module_t*>(module);
    dev->common.close    = poll__close;
    dev->activate        = poll__activate;
    dev->setDelay        = poll__setDelay;
    dev->poll            = poll__poll;

    *device = &dev->common;
    return 0;
}
