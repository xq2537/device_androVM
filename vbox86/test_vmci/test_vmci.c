#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define linux
#include "vmci_sockets.h"
#undef linux

int main(int argc, char *argv) {
    struct sockaddr_vm addr;
    socklen_t alen;
    int s;

    printf("VMCI test starting\n");


    int afVMCI = VMCISock_GetAFValue();
    printf("VMCI Socket value = %d\n", afVMCI);
    s = socket(afVMCI, SOCK_STREAM, 0);
    if(s < 0) {
        printf("Socket error...\n");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.svm_family = afVMCI;
    addr.svm_cid = 2;
    addr.svm_port = htons(22468);

    if(connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        printf("Connect error...\n");
        return -1;
    }

    printf("Connect OK :-)\n");
    return 0;
}

