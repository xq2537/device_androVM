#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <errno.h>
#include <cutils/properties.h>

#define HOST_IF "eth0"

int main(int argc, char *argv[]) {
    int fd_socket;
    struct ifreq ifr;

    fd_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_socket < 0) {
        fprintf(stderr, "Unable to create socket\n");
        exit(-1);
    }

    ifr.ifr_addr.sa_family = AF_INET;

    if (argc>1)
        strncpy(ifr.ifr_name, argv[1], IFNAMSIZ-1);
    else
        strncpy(ifr.ifr_name, HOST_IF, IFNAMSIZ-1);

    while (ioctl(fd_socket, SIOCGIFADDR, &ifr) < 0) {
        fprintf(stderr, "Error in ioctl\n");
        sleep(5);
    }

    close(fd_socket);

    struct sockaddr_in host_ip;

    memcpy(&host_ip, &ifr.ifr_addr, sizeof(struct sockaddr_in));

    printf("Interface IP address is %s\n", inet_ntoa(host_ip.sin_addr));

    host_ip.sin_addr.s_addr = host_ip.sin_addr.s_addr & 0x00FFFFFF | 0x01000000;

    printf("Remote IP address is %s\n", inet_ntoa(host_ip.sin_addr));

    property_set("androVM.server.ip", inet_ntoa(host_ip.sin_addr));

    exit(0);
}
