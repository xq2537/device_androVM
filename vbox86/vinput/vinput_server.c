#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <cutils/properties.h>

#include <linux/input.h>
#include <linux/uinput.h>

int main(int argc, char *argv[]) {
    int uinp_fd;
    struct uinput_user_dev uinp;
    struct input_event event;
    int ssocket;
    struct sockaddr_in srv_addr;
    long haddr;
    int i;

    // Create input devices
    if (!(uinp_fd = open("/dev/uinput", O_WRONLY|O_NDELAY))) {
	fprintf(stderr, "Unable to open /dev/uinput !\n");
	exit(-1);
    }

    memset(&uinp, 0, sizeof(uinp));
    strncpy(uinp.name, "androVM Virtual Input", UINPUT_MAX_NAME_SIZE);
    uinp.id.vendor=0x1234;
    uinp.id.product=1;
    uinp.id.version=1;
    uinp.absmin[0]=0;
    uinp.absmax[0]=1024;
    uinp.absmin[1]=0;
    uinp.absmax[1]=600;
    ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinp_fd, UI_SET_EVBIT, EV_REL);
    ioctl(uinp_fd, UI_SET_RELBIT, REL_X);
    ioctl(uinp_fd, UI_SET_RELBIT, REL_Y);
    ioctl(uinp_fd, UI_SET_EVBIT, EV_ABS);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_X);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_Y);
    for (i=0;i<256;i++)
	ioctl(uinp_fd, UI_SET_KEYBIT, i);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE);
    write(uinp_fd, &uinp, sizeof(uinp));
    if (ioctl(uinp_fd, UI_DEV_CREATE)) {
	fprintf(stderr, "Unable to create uinput device...\n");
	exit(-1);
    }

    // Listen 
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(22469);

    if ((ssocket = socket(AF_INET, SOCK_STREAM, 0))<0) {
	fprintf(stderr, "Unable to create socket\n");
	exit(-1);
    }

    while (1) {
        int csocket;

        if (bind(ssocket, (struct sockaddr *)&srv_addr, sizeof(srv_addr))<0) {
            fprintf(stderr, "Unable to bind socket, errno=%d\n", errno);
            sleep(1);
            continue;
        }

        if (listen(ssocket, 5)<0) {
            fprintf(stderr, "Unable to listen to socket, errno=%d\n", errno);
            sleep(1);
            continue;
        }
       
        while (1) {
	    if ((csocket=accept(ssocket, NULL, 0))<0) {
	        fprintf(stderr, "Unable to accept conn, errno=%d\n", errno);
                sleep(1);
                continue;
            }

            if (fork()==0) {
	        FILE *f_sock;
#define BUFSIZE 256
	        char mbuf[BUFSIZE];

	        f_sock = fdopen(csocket, "r");
	        if (!f_sock) {
	            fprintf(stderr, "Unable to have fdsock on socket...\n");
	            close(csocket);
                    exit(1);
	        }

	        while (fgets(mbuf, BUFSIZE, f_sock)) {
	            char *pcmd, *p1, *p2, *pb, *pe;

	            pcmd = p1 = p2 = NULL;
	            if (pe=strchr(mbuf,'\n'))
		        *pe='\0';
	            pcmd=pb=mbuf;
	            if (pb && (pe=strchr(pb,':'))) {
	 	        *pe='\0';
		        p1=pb=++pe;
	            }
	            if (pb && (pe=strchr(pb,':'))) {
		        *pe='\0';
		        p2=pb=++pe;
	            }
	            if (!pcmd) 
		        continue;
	            if (!strcmp(pcmd,"MOUSE")) {
		        if (!p1 || !p2)
		            continue;
		        memset(&event, 0, sizeof(event));
		        gettimeofday(&event.time, NULL);
		        event.type = EV_REL;
		        event.code = REL_X;
		        event.value = atoi(p1);
		        write(uinp_fd, &event, sizeof(event));
		        event.type = EV_REL;
		        event.code = REL_Y;
		        event.value = atoi(p2);
		        write(uinp_fd, &event, sizeof(event));
		        event.type = EV_SYN;
	 	        event.code = SYN_REPORT;
		        event.value = 0;
	 	        write(uinp_fd, &event, sizeof(event));
	            }
	            else if (!strcmp(pcmd,"MSBPR")) {
		        if (!p1 || !p2) 
		            continue;
		        memset(&event, 0, sizeof(event));
		        gettimeofday(&event.time, NULL);
		        event.type = EV_KEY;
		        event.code = BTN_MOUSE;
		        event.value = 1;
		        write(uinp_fd, &event, sizeof(event));
		        event.type = EV_SYN;
		        event.code = SYN_REPORT;
		        event.value = 0;
		        write(uinp_fd, &event, sizeof(event));
	            }
	            else if (!strcmp(pcmd,"MSBRL")) {
		        if (!p1 || !p2) 
		            continue;
		        memset(&event, 0, sizeof(event));
		        gettimeofday(&event.time, NULL);
		        event.type = EV_KEY;
		        event.code = BTN_MOUSE;
		        event.value = 0;
		        write(uinp_fd, &event, sizeof(event));
		        event.type = EV_SYN;
		        event.code = SYN_REPORT;
		        event.value = 0;
		        write(uinp_fd, &event, sizeof(event));
	            }
	            else if (!strcmp(pcmd,"KBDPR")) {
		        if (!p1 || !p2) 
		            continue;
	  	        memset(&event, 0, sizeof(event));
		        gettimeofday(&event.time, NULL);
		        event.type = EV_KEY;
		        event.code = atoi(p1);
		        event.value = 1;
		        write(uinp_fd, &event, sizeof(event));
		        event.type = EV_SYN;
		        event.code = SYN_REPORT;
		        event.value = 0;
		        write(uinp_fd, &event, sizeof(event));
	            }
	            else if (!strcmp(pcmd,"KBDRL")) {
		        if (!p1 || !p2) 
		            continue;
		        memset(&event, 0, sizeof(event));
		        gettimeofday(&event.time, NULL);
		        event.type = EV_KEY;
		        event.code = atoi(p1);
		        event.value = 0;
		        write(uinp_fd, &event, sizeof(event));
		        event.type = EV_SYN;
		        event.code = SYN_REPORT;
		        event.value = 0;
		        write(uinp_fd, &event, sizeof(event));
	            }
	        }

	        close(csocket);
                exit(0);
            }
        }
    }

    return 0;    
}
