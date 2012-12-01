#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cutils/properties.h>

#define UDP_PORT_REQ 22468
#define UDP_PORT_RESP 22469
#define BROADCAST_IF "eth0"

int main(int argc, char *argv[]) {
    int udp_socket;
    int opt_broadcast=1;
    struct sockaddr_in sendaddr;
    struct sockaddr_in recvaddr;

    if ((udp_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
	fprintf(stderr, "Unable to create socket\n");
	exit(-1);
    }

    if ((setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &opt_broadcast, sizeof(opt_broadcast))) < 0) {
	fprintf(stderr, "Unable to setsockopt broadcast\n");
	exit(-1);
    }

    const char device[] = BROADCAST_IF;
    if ((setsockopt(udp_socket, SOL_SOCKET, SO_BINDTODEVICE, (void *)device, sizeof(device)))<0) 
	fprintf(stderr, "Unable to setsockopt bindtodevice\n");

    sendaddr.sin_family = AF_INET;
    sendaddr.sin_port = htons(UDP_PORT_RESP);
    sendaddr.sin_addr.s_addr = INADDR_ANY;
    memset(sendaddr.sin_zero,0,sizeof(sendaddr.sin_zero));

    if (bind(udp_socket, (struct sockaddr *)&sendaddr, sizeof(sendaddr))<0) {
	fprintf(stderr, "Unable to bind local socket\n");
	exit(-1);
    }

    struct sockaddr_in raddr;
    socklen_t raddrlen;

    for (;;) {
        recvaddr.sin_family = AF_INET;
        recvaddr.sin_port = htons(UDP_PORT_REQ);
        recvaddr.sin_addr.s_addr = INADDR_BROADCAST;
        memset(recvaddr.sin_zero,0,sizeof(recvaddr.sin_zero));

        if (sendto(udp_socket, "GETIP", 5, 0, (struct sockaddr *)&recvaddr, sizeof(recvaddr))<0) {
	    fprintf(stderr, "Error sending broadcast packet\n");
	    sleep(5);
	    continue;
        }

        char rbuf[256];
	int i;
        int res;

        raddr.sin_family = AF_INET;
        raddr.sin_port = htons(UDP_PORT_RESP);
        raddr.sin_addr.s_addr = INADDR_ANY;
        memset(raddr.sin_zero,0,sizeof(raddr.sin_zero));
        raddrlen=sizeof(raddr);

	for (i=0;i<3;i++) {
            if ((res=recvfrom(udp_socket, rbuf, 255, MSG_DONTWAIT, (struct sockaddr *)&raddr, &raddrlen))>=0)
                break;
	    sleep(1);
        }

        if (res<0) {
            fprintf(stderr,"Timeout receiving IP address, resending broadcast packet...\n");
            continue;
        }
        if (raddrlen<=0) {
	    fprintf(stderr, "Couldn't get IP address in recvfrom, resending broadcast packet in few seconds\n");
            sleep(5);
            continue;
        }

        break;
    }

    printf("Remote IP address is %s\n", inet_ntoa(raddr.sin_addr));

    property_set("androVM.server.ip", inet_ntoa(raddr.sin_addr));

    exit(0);
}
