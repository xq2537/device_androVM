#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <cutils/properties.h>

#define MAIN_PORT 25000
#define OPENGL_DATA_PORT 22468

struct conn_duo {
    int local_socket;
    int host_socket;
};

#define MAX_FREE_SOCKETS 5

#define BUFF_SIZE (4*1024*1024)

#define OPENGL_START_COMMAND 1001
#define OPENGL_PING 1002
#define OPENGL_PONG 1003

#define HEARTBEAT_PERIOD 3

static int copy_socket(int fd_read, int fd_write, char *buff) {
    int rsize,towrite,wsize;

    rsize = read(fd_read, buff, BUFF_SIZE);
    if (rsize<=0) {
        close(fd_read);
        close(fd_write);
        return -1;
    }

    towrite = rsize;
  
    while ((wsize=write(fd_write, buff, towrite))>0)
        towrite -= wsize;
    if (towrite>0) {
        close(fd_read);
        close(fd_write);
        return -1;
    }

    return 0;
}

static void *conn_thread(void *arg) {
    struct conn_duo *cd = (struct conn_duo *)arg;
    char *buff;

    buff = (char *)malloc(BUFF_SIZE);
    if (!buff) {
        fprintf(stderr, "Thread %u: Unable to alloc %d bytes\n", pthread_self(), BUFF_SIZE);
        return NULL;
    }

    while (1) {
        fd_set set_read;
        int nfds;
        int r;

        FD_ZERO(&set_read);
        FD_SET(cd->local_socket, &set_read);
        FD_SET(cd->host_socket, &set_read);

        if (cd->local_socket > cd->host_socket)
            nfds = cd->local_socket+1;
        else
            nfds = cd->host_socket+1;

        if ((r=select(nfds+1, &set_read, NULL, NULL, NULL))<0) {
            fprintf(stderr, "Thread %u: Error in select(), errno=%d\n", pthread_self(), errno);
            if (errno==EINTR)
                continue;
            break;
        }

        if ((FD_ISSET(cd->local_socket, &set_read)) && (copy_socket(cd->local_socket, cd->host_socket, buff)<0))
            break;

        if ((FD_ISSET(cd->host_socket, &set_read)) && (copy_socket(cd->host_socket, cd->local_socket, buff)<0))
            break;
    }
    fprintf(stderr, "Thread %u: Stopping thread\n", pthread_self());

    free(buff);

    close(cd->local_socket);
    close(cd->host_socket);
    free(cd);

    return NULL;
}


int main(int argc, char *argv[]) {
    int ssocket, main_socket;
    struct sockaddr_in srv_addr;
    long haddr;
    int i;
    int free_local_sockets[MAX_FREE_SOCKETS];
    int nb_free_local=0;

    // Listen for main connection
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_port = htons(MAIN_PORT);

    if ((ssocket = socket(AF_INET, SOCK_STREAM, 0))<0) {
	fprintf(stderr, "Unable to create socket\n");
	exit(-1);
    }

    int yes = 1;
    setsockopt(ssocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(ssocket, (struct sockaddr *)&srv_addr, sizeof(srv_addr))<0) {
        fprintf(stderr, "Unable to bind socket, errno=%d\n", errno);
        exit(-1);
    }

    if (listen(ssocket, 1)<0) {
        fprintf(stderr, "Unable to listen to socket, errno=%d\n", errno);
        exit(-1);
    }

    main_socket=accept(ssocket, NULL, 0);
    if (main_socket<0) {
        fprintf(stderr, "Unable to accept socket for main conection, errno=%d\n", errno);
        exit(-1);
    }
    close(ssocket);

    unsigned int cmd;
    if (read(main_socket, &cmd, sizeof(cmd))!=sizeof(cmd)) {
        fprintf(stderr, "Unable to read cmd from main connection\n");
        exit(-1);
    }
    if (cmd!=OPENGL_START_COMMAND) {
        fprintf(stderr, "Unknown cmd : %d\n", cmd);
        exit(-1);
    }
    unsigned int data_port = OPENGL_DATA_PORT;
    if (write(main_socket, &data_port, sizeof(data_port))!=sizeof(data_port)) {
        fprintf(stderr, "Unable to write data port to main connection\n");
        exit(-1);
    }

    // Listen for data connection
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_port = htons(OPENGL_DATA_PORT);

    if ((ssocket = socket(AF_INET, SOCK_STREAM, 0))<0) {
	fprintf(stderr, "Unable to create socket\n");
	exit(-1);
    }

    yes = 1;
    setsockopt(ssocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(ssocket, (struct sockaddr *)&srv_addr, sizeof(srv_addr))<0) {
        fprintf(stderr, "Unable to bind socket, errno=%d\n", errno);
        exit(-1);
    }

    if (listen(ssocket, 5)<0) {
        fprintf(stderr, "Unable to listen to socket, errno=%d\n", errno);
        exit(-1);
    }

    while (1) {
        struct conn_duo *new_cd;
        pthread_t new_thread_id;
        int csocket;
        struct sockaddr_in cli_addr;
        int cli_addr_len;
        fd_set fds_read;
        struct timeval tv_timeout;
        int sl;
        int lasttime_ping=0;
        int lasttime_pong=0;

        FD_ZERO(&fds_read);
        FD_SET(ssocket, &fds_read);
        FD_SET(main_socket, &fds_read);
        tv_timeout.tv_sec = HEARTBEAT_PERIOD;
        tv_timeout.tv_usec = 0;

        printf("Let's select()\n");
        sl=select(ssocket+1, &fds_read, NULL, NULL, &tv_timeout);
        printf("select() returns %d\n", sl);

        int curtime = time(NULL);
        if ((curtime - lasttime_ping) > HEARTBEAT_PERIOD) {
            int nop;

            if ((lasttime_ping - lasttime_pong) > 0) {
                // main socket closed, exiting...
                exit(0);
            }

            printf("Will ping host...\n");
            nop = OPENGL_PING;
            if (write(main_socket, &nop, sizeof(nop))!=sizeof(nop)) {
                // main socket closed, exiting...
                exit(0);
            }
            lasttime_ping = curtime;
        }

        if ((sl<0) && (errno!=EINTR))
            exit(1); 
        if (sl<0)
            continue;
        if (!sl) {
            //Timeout - flush localhost 'free cons'
            int c;

            for (c=0;c<nb_free_local;c++)
                close(free_local_sockets[c]);
            nb_free_local = 0;
            continue;
        }

        if (FD_ISSET(main_socket, &fds_read)) {
            int nop;

            if (read(main_socket, &nop, sizeof(nop))<=0) {
                // main socket closed, exiting...
                exit(0);
            }
            switch (nop) {
             case OPENGL_PING:
                nop = OPENGL_PONG;
                if (write(main_socket, &nop, sizeof(nop))!=sizeof(nop)) {
                    // main socket closed, exiting...
                    exit(0);
                }
                break;
             case OPENGL_PONG:
                lasttime_pong = time(NULL);
                break;
            }
        }

        if (FD_ISSET(ssocket, &fds_read)) {
            if ((csocket=accept(ssocket, NULL, 0))<0) {
	        fprintf(stderr, "Unable to accept conn, errno=%d\n", errno);
                continue;
            }

            cli_addr_len = sizeof(cli_addr);
            if (getpeername(csocket, (struct sockaddr *)&cli_addr, &cli_addr_len)) {
                fprintf(stderr, "Unable to get peer address on socket %d, errno=%d\n", csocket, errno);
                close(csocket);
                continue;
            }

#ifdef WIN32
            DWORD opt_nodelay;
#else
            int opt_nodelay;
#endif
            opt_nodelay = 1;
            setsockopt(csocket, IPPROTO_TCP, TCP_NODELAY, &opt_nodelay, sizeof(opt_nodelay));

            if ((ntohl(cli_addr.sin_addr.s_addr) & 0xFF000000) == (INADDR_LOOPBACK & 0xFF000000)) {
                printf("New connection from local...\n");
                if (nb_free_local >= MAX_FREE_SOCKETS) {
                    fprintf(stderr, "Maximum number of free local sockets reached\n");
                    close(csocket);
                    continue;
                }
                free_local_sockets[nb_free_local++] = csocket;

#define TCPCLI_NEW 1
                unsigned int clicmd = TCPCLI_NEW;
                if (write(main_socket, &clicmd, sizeof(clicmd))!=sizeof(clicmd)) {
                    fprintf(stderr, "Unable to write new conn command to main connection\n");
                    exit(-1);
                }
            }
            else {
                printf("New connection from host...\n");
                if (!nb_free_local) {
                    fprintf(stderr, "No free local socket, that's abnormal...\n");
                    close(csocket);
                    continue;
                }
                new_cd = (struct conn_duo *)malloc(sizeof(struct conn_duo));
                if (!new_cd) {
                    fprintf(stderr, "Out of memory\n");
                    close(csocket);
                    continue;
                }
                new_cd->host_socket = csocket;
                new_cd->local_socket = free_local_sockets[0];
                if (--nb_free_local > 0)
                    memmove(free_local_sockets, free_local_sockets+1, nb_free_local*sizeof(struct conn_duo));
                if (pthread_create(&new_thread_id, NULL, conn_thread, (void *)new_cd)) {
                    fprintf(stderr, "Unable to create thread, errno=%d\n", errno);
                    close(new_cd->local_socket);
                    close(new_cd->host_socket);
                    free(new_cd);
                    continue;
                }
            }
        }
    }

    return 0;    
}
