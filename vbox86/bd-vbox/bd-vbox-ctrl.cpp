#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <VBox/VBoxGuest.h>
#include <iprt/err.h>
#include <VBox/HostServices/GuestControlSvc.h>
#include <VBGLR3Internal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/un.h>

#include <pthread.h>

#define LISTEN_TCP_PORT 22468
//#define USE_UNIX_SOCKET 1

using namespace guestControl;

int g_ClientID;

struct th_message;

struct thread_conn_info {
    pthread_t thd_in;
    pthread_t thd_out;
    int socket;
    struct th_message *ctrl_message_in;
    pthread_mutex_t ctrl_message_in_mutex;
    pthread_cond_t ctrl_message_in_cond;
    struct thread_conn_info *next;
};

struct thread_conn_info *tci_head = NULL;

void add_thread_conn_info(struct thread_conn_info *p_tci)
{
    p_tci->next = tci_head;
    tci_head = p_tci;
}

void del_thread_conn_info(struct thread_conn_info *p_tci)
{
    struct thread_conn_info *p, *q;

    p = q = tci_head;
    while (p) {
	if (p == p_tci) {
	    if (p == tci_head)
		tci_head = p->next;
	    else {
		q->next = p->next;
	    }
	}
	q = p;
	p = p->next;
    }
}

struct thread_conn_info *get_thread_conn_info(int cid)
{
    struct thread_conn_info *p_tci;

    for (p_tci=tci_head; p_tci; p_tci=p_tci->next) {
	if (p_tci->socket == cid)
	    return p_tci;
    }
    return NULL;
}    
    
struct th_message {
    int cid;
    int size;
    int term;
    char *datas;
    struct th_message *next;
};

struct th_message *ctrl_message_out = NULL;
pthread_mutex_t ctrl_message_out_mutex;
pthread_cond_t ctrl_message_out_cond;

void push_message(struct th_message **p_head, pthread_mutex_t *p_mutex, pthread_cond_t *p_cond, struct th_message *msg) {
    pthread_mutex_lock(p_mutex);
    msg->next = NULL;
    if (!*p_head) {
	*p_head = msg;
	pthread_mutex_unlock(p_mutex);
	pthread_cond_signal(p_cond);
        return;
    }
    struct th_message *m=*p_head;
    while (m->next)
	m=m->next;
    m->next = msg;
    pthread_mutex_unlock(p_mutex);
    pthread_cond_signal(p_cond);
}

struct th_message *pop_message(struct th_message **p_head, pthread_mutex_t *p_mutex, pthread_cond_t *p_cond) {
    pthread_mutex_lock(p_mutex);
    while (!*p_head) {
	pthread_mutex_unlock(p_mutex);
	pthread_cond_wait(p_cond, p_mutex);
    }
    struct th_message *m = *p_head;
    *p_head = (*p_head)->next;
    pthread_mutex_unlock(p_mutex);
    return m;
}

inline void push_message_in(struct thread_conn_info *p_tci, struct th_message *msg) {
    push_message(&p_tci->ctrl_message_in, &p_tci->ctrl_message_in_mutex, &p_tci->ctrl_message_in_cond, msg);
}

inline struct th_message *pop_message_in(struct thread_conn_info *p_tci) {
    return pop_message(&p_tci->ctrl_message_in, &p_tci->ctrl_message_in_mutex, &p_tci->ctrl_message_in_cond);
}

inline void push_message_out(struct th_message *msg) {
    push_message(&ctrl_message_out, &ctrl_message_out_mutex, &ctrl_message_out_cond, msg);
}

struct th_context_out {
    int cid;
    int context;
    int handle;
    int flags;
    struct th_context_out *next;   
};

struct th_context_out *ctrl_context_out = NULL;

void push_context_out(struct th_context_out *ctxt) {
    pthread_mutex_lock(&ctrl_message_out_mutex);
    ctxt->next = NULL;
    if (!ctrl_context_out) {
	ctrl_context_out = ctxt;
	pthread_mutex_unlock(&ctrl_message_out_mutex);
	pthread_cond_signal(&ctrl_message_out_cond);
        return;
    }
    struct th_context_out *c=ctrl_context_out;
    while (c->next)
	c=c->next;
    c->next = ctxt;
    pthread_mutex_unlock(&ctrl_message_out_mutex);
    pthread_cond_signal(&ctrl_message_out_cond);
}

void find_next_context_message_out(struct th_context_out **pp_ctxt, struct th_message **pp_msg)
{
    struct th_context_out *p_ctxt, *q_ctxt;
    struct th_message *p_msg, *q_msg;

    pthread_mutex_lock(&ctrl_message_out_mutex);
    while (1) {
	for (p_ctxt=ctrl_context_out,q_ctxt=NULL; p_ctxt; p_ctxt=p_ctxt->next) {
	    for (p_msg=ctrl_message_out,q_msg=NULL; p_msg; p_msg=p_msg->next) {
		if (p_ctxt->cid == p_msg->cid) {
		    if (q_ctxt) 
			q_ctxt->next = p_ctxt->next;
		    else
			ctrl_context_out = p_ctxt->next;
		    if (q_msg)
			q_msg->next = p_msg->next;
		    else
			ctrl_message_out = p_msg->next;

		    pthread_mutex_unlock(&ctrl_message_out_mutex);
		    *pp_ctxt = p_ctxt;
		    *pp_msg = p_msg;

		    return;
		}
		q_msg = p_msg;
	    }
	    q_ctxt = p_ctxt;
	}
	pthread_mutex_unlock(&ctrl_message_out_mutex);
	pthread_cond_wait(&ctrl_message_out_cond, &ctrl_message_out_mutex);
    }
}

int connectControl(int *clientID)
{
    VBoxGuestHGCMConnectInfo Info;
    memset(&Info, 0, sizeof(Info));
    Info.result = VERR_WRONG_ORDER;
    Info.Loc.type = VMMDevHGCMLoc_LocalHost_Existing;
    strcpy(Info.Loc.u.host.achName, "VBoxGuestControlSvc");
    Info.u32ClientID = UINT32_MAX;  /* try make valgrind shut up. */

    int iofile = open("/dev/vboxguest",O_RDWR);
    if (!iofile) {
        fprintf(stderr,"Unable to open /dev/vboxguest errno=%d\n",errno);
        return -1;
    }

    int rc = ioctl(iofile, VBOXGUEST_IOCTL_HGCM_CONNECT, &Info);
    if (rc) {
        fprintf(stderr,"Error in ioctl errno=%d\n", errno);
        return -2;
    }

    rc = Info.result;
    if (RT_FAILURE(rc)) {
        fprintf(stderr,"Connect NOK, rc=%d\n",rc);
	return -1;
    }

    *clientID = Info.u32ClientID;

    return iofile;
}

inline void setInt32(HGCMFunctionParameter *pParm, uint32_t u32)
{
    pParm->type = VMMDevHGCMParmType_32bit;
    pParm->u.value64 = 0;
    pParm->u.value32 = u32;
}

inline void setPtr(HGCMFunctionParameter *pParm, void *pv, uint32_t cb)
{
    pParm->type = VMMDevHGCMParmType_LinAddr;
    pParm->u.Pointer.size = cb;
    pParm->u.Pointer.u.linearAddr = (uintptr_t)pv;
}

int getNextHostMsg(int iofile, int clientID, int *puMsg, int *puNumParms)
{
    VBoxGuestCtrlHGCMMsgType Msg;

    Msg.hdr.result = VERR_WRONG_ORDER;
    Msg.hdr.u32ClientID = clientID;
    Msg.hdr.u32Function = GUEST_GET_HOST_MSG;
    Msg.hdr.cParms = 2;
 
    setInt32(&Msg.msg, 0);
    setInt32(&Msg.num_parms, 0);   

    int rc = ioctl(iofile, VBOXGUEST_IOCTL_HGCM_CALL(sizeof(Msg)), &Msg);
    if (rc) {
        fprintf(stderr,"Error in ioctl errno=%d\n", errno);
        return -2;
    }

    *puMsg = Msg.msg.u.value32;
    *puNumParms = Msg.num_parms.u.value32;

    return 0;
}

int ReportStatus(int iofile, int ClientID, int Context, int PID, int Status, int Flags, void *pvData, int cbData)
{
    VBoxGuestCtrlHGCMMsgExecStatus Msg;

    Msg.hdr.result = VERR_WRONG_ORDER;
    Msg.hdr.u32ClientID = ClientID;
    Msg.hdr.u32Function = GUEST_EXEC_SEND_STATUS;
    Msg.hdr.cParms = 5;

    setInt32(&Msg.context, Context);
    setInt32(&Msg.pid, PID);
    setInt32(&Msg.status, Status);
    setInt32(&Msg.flags, Flags);
    setPtr(&Msg.data, pvData, cbData);

    //printf("ReportStatus : context=%d pid=%d status=%d\n", Context, PID, Status);
    int rc = ioctl(iofile, VBOXGUEST_IOCTL_HGCM_CALL(sizeof(Msg)), &Msg);
    if (rc) {
        fprintf(stderr,"Error in ioctl ReportStatus errno=%d\n", errno);
        return -1;
    }

    return 0;
}

void handleExecCmd(int iofile, int ClientID, int Context, char *szCmd, char *szArgs)
{
    static int main_cid=20000;
    int cid=0;

    if (szArgs)
        sscanf(szArgs, "%d", &cid);
    else
	fprintf(stderr,"handleExecCmd(): szArgs NULL !\n");
    if (!cid)
	cid=main_cid;
    //printf("handleExecCmd: got cid=%d\n", cid);
    ReportStatus(iofile, ClientID, Context, cid, PROC_STS_STARTED, 0, NULL, 0);
}

int handleHostExecCmd(int iofile, int ClientID, int cParms)
{
    VBoxGuestCtrlHGCMMsgExecCmd Msg;
    int Context;
    char szCmd[256]="";
    int cbCmd=255;
    int Flags;
    char szArgs[256]="";
    int cbArgs=255;
    int cArgs;
    char szEnv[256]="";
    int cbEnv=255;
    int cEnvVars;
    char szUser[256]="";
    int cbUser=255;
    char szPassword[256]="";
    int cbPassword=255;
    int MsTimeLimit;
    

    Msg.hdr.result = VERR_WRONG_ORDER;
    Msg.hdr.u32ClientID = ClientID;
    Msg.hdr.u32Function = GUEST_GET_HOST_MSG;
    Msg.hdr.cParms = 11;

    setInt32(&Msg.context, 0);
    setPtr(&Msg.cmd, szCmd, cbCmd);
    setInt32(&Msg.flags, 0);
    setInt32(&Msg.num_args, 0);
    setPtr(&Msg.args, szArgs, cbArgs);
    setInt32(&Msg.num_env, 0);
    setInt32(&Msg.cb_env, 0);
    setPtr(&Msg.env, szEnv, cbEnv);
    setPtr(&Msg.username, szUser, cbUser);
    setPtr(&Msg.password, szPassword, cbPassword);
    setInt32(&Msg.timeout, 0);

    int rc = ioctl(iofile, VBOXGUEST_IOCTL_HGCM_CALL(sizeof(Msg)), &Msg);
    if (rc) {
        fprintf(stderr,"Error in ioctl errno=%d\n", errno);
        return -1;
    }

    Context = Msg.context.u.value32;
    Flags = Msg.flags.u.value32;
    cArgs = Msg.flags.u.value32;
    cEnvVars = Msg.flags.u.value32;
    cbEnv = Msg.cb_env.u.value32;
    MsTimeLimit = Msg.timeout.u.value32;

    //printf("Got Exec cmd - context=%d cmd=%s args=%s\n", Context, szCmd, szArgs);
    handleExecCmd(iofile, ClientID, Context, szCmd, szArgs);

    return 0;
}

int handleGetCmdOutput(int iofile, int ClientID, int cParms)
{
    VBoxGuestCtrlHGCMMsgExecOut Msg;
    int Context, PID, Handle, Flags;

    Msg.hdr.result = VERR_WRONG_ORDER;
    Msg.hdr.u32ClientID = ClientID;
    Msg.hdr.u32Function = GUEST_GET_HOST_MSG;
    Msg.hdr.cParms = cParms;

    setInt32(&Msg.context, 0);
    setInt32(&Msg.pid, 0);
    setInt32(&Msg.handle, 0);
    setInt32(&Msg.flags, 0);

    int rc = ioctl(iofile, VBOXGUEST_IOCTL_HGCM_CALL(sizeof(Msg)), &Msg);
    if (rc) {
        fprintf(stderr,"Error in ioctl GETCMDOUTPUT errno=%d\n", errno);
        return -1;
    }

    Context = Msg.context.u.value32;
    PID = Msg.pid.u.value32;
    Handle = Msg.handle.u.value32;
    Flags = Msg.flags.u.value32;

    // Create & Push Context
    struct th_context_out *ctxt;

    ctxt = (struct th_context_out *)malloc(sizeof(struct th_context_out));
    if (!ctxt) {
	fprintf(stderr, "Malloc error on struct th_context\n");
	return -1;
    }
    ctxt->cid = PID;
    ctxt->context = Context;
    ctxt->handle = Handle;
    ctxt->flags = Flags;
    //printf("Push Context out with PID=%d\n", PID);
    push_context_out(ctxt);

    return 0;
}

void *thread_GuestControl_out(void *p_arg)
{
    int fd_ioctl = (int)p_arg;

    for (;;) {
	struct th_context_out *ctxt;
	struct th_message *msg;

	find_next_context_message_out(&ctxt, &msg);
	if (!ctxt) {
	    fprintf(stderr, "Get context NULL, abnormal thing !\n");
	    continue;
	}
	if (!msg) {
	    fprintf(stderr, "Get message NULL, abnormal thing !\n");
	    continue;
	}

	//printf("[THREAD_GC_OUT] Got context out with cid=%d, context=%d\n", ctxt->cid, ctxt->context);
	//printf("[THREAD_GC_OUT] Got message out with cid=%d, size=%d\n", msg->cid, msg->size);

        // Send Data
	int rc;

        VBoxGuestCtrlHGCMMsgExecOut MsgSend;

        MsgSend.hdr.result = VERR_WRONG_ORDER;
        MsgSend.hdr.u32ClientID = g_ClientID;
        MsgSend.hdr.u32Function = GUEST_EXEC_SEND_OUTPUT;
        MsgSend.hdr.cParms = 5;
   
        setInt32(&MsgSend.context, ctxt->context);
        setInt32(&MsgSend.pid, ctxt->cid);
        setInt32(&MsgSend.handle, ctxt->handle); 
        setInt32(&MsgSend.flags, ctxt->flags);
	if (msg->size > 0) 
            setPtr(&MsgSend.data, msg->datas, msg->size);
	else 
	    setPtr(&MsgSend.data, NULL, 0);

	//printf("Sending %d bytes datas to cid %d\n", msg->size, ctxt->cid);

        rc = ioctl(fd_ioctl, VBOXGUEST_IOCTL_HGCM_CALL(sizeof(MsgSend)), &MsgSend);
	if (msg->size == 0) 
	    ReportStatus(fd_ioctl, g_ClientID, ctxt->context, ctxt->cid, PROC_STS_TEN, 0, NULL, 0);

	free(ctxt);
	if (msg->datas) 
	    free(msg->datas);
	free(msg);

        if (rc) {
            fprintf(stderr,"Error in ioctl SENDOUTPUT errno=%d\n", errno);
            return NULL;
        }

    }

    return NULL;
}

int handleGetCmdInput(int iofile, int ClientID, int cParms)
{
    VBoxGuestCtrlHGCMMsgExecIn Msg;
    int Context, PID, Flags, Size;
    static char *datas_in = NULL;
#define SIZE_BUFFER_IN 512000

    if (!datas_in) {
	datas_in = (char *)malloc(SIZE_BUFFER_IN);
	if (!datas_in) {
	    fprintf(stderr, "handleGetCmdInput(): Unable to alloc buffer memory\n");
	    return -1;
	}
    }

    Msg.hdr.result = VERR_WRONG_ORDER;
    Msg.hdr.u32ClientID = ClientID;
    Msg.hdr.u32Function = GUEST_GET_HOST_MSG;
    Msg.hdr.cParms = cParms;

    setInt32(&Msg.context, 0);
    setInt32(&Msg.pid, 0);
    setInt32(&Msg.flags, 0);
    setPtr(&Msg.data, datas_in, SIZE_BUFFER_IN);
    setInt32(&Msg.size, 0);

    int rc = ioctl(iofile, VBOXGUEST_IOCTL_HGCM_CALL(sizeof(Msg)), &Msg);
    if (rc) {
        fprintf(stderr,"Error in ioctl GETCMDHOSTINPUT errno=%d\n", errno);
        return -1;
    }
    int rc2 = Msg.hdr.result;
    if (RT_FAILURE(rc2)) {
	fprintf(stderr,"Error in GetCmdHostInput, rc=%X\n", rc2);
	return -1;
    }

    Context = Msg.context.u.value32;
    PID = Msg.pid.u.value32;
    Flags = Msg.pid.u.value32;
    Size = Msg.size.u.value32;

    //printf("Got Input message size=%d for pid %d\n", Size, PID);

    struct VBoxGuestCtrlHGCMMsgExecStatusIn MsgSend;

    MsgSend.hdr.result = VERR_WRONG_ORDER;
    MsgSend.hdr.u32ClientID = ClientID;
    MsgSend.hdr.u32Function = GUEST_EXEC_SEND_INPUT_STATUS;
    MsgSend.hdr.cParms = 5; 

    setInt32(&MsgSend.context, Context);
    setInt32(&MsgSend.pid, PID);
    setInt32(&MsgSend.status, INPUT_STS_WRITTEN);
    setInt32(&MsgSend.flags, 0);
    setInt32(&MsgSend.written, Size);

    rc = ioctl(iofile, VBOXGUEST_IOCTL_HGCM_CALL(sizeof(MsgSend)), &MsgSend);
    if (rc) {
        fprintf(stderr,"Error in ioctl STATUSIN errno=%d\n", errno);
        return -1;
    }

    //if (Size>=4) {
//	printf("First 'int' : %d\n", *((int *)datas_in));
 //   }

    thread_conn_info *p_tci;

    p_tci = get_thread_conn_info(PID);
    if (!p_tci) {
	fprintf(stderr, "Can't find thread_conn_info for pid %d\n", PID);
	return -1;
    }

    struct th_message *msg;

    msg = (struct th_message *)malloc(sizeof(struct th_message));
    if (!msg) {
	fprintf(stderr, "Can't alloc memory for th_message\n");
	return -1;
    }
    msg->cid = PID;
    msg->size = Size;
#if 0
    if (Flags & INPUT_FLAG_EOF) {
	fprintf(stderr, "Got EOF on INPUT\n");
	msg->term = 1;
    }
    else
	msg->term = 0;
#endif 
    msg->term = 0;
    msg->datas = (char *)malloc(Size);
    if (!msg->datas) {
	fprintf(stderr, "Unable to alloc %d bytes for in message\n", Size);
	return -1;
    }
    memcpy(msg->datas, datas_in, Size);

    push_message_in(p_tci, msg);

    return 0;
}

void *thread_GuestControl(void *p_arg)
{
    int fd_ioctl;
    pthread_t thd_gc_out;

    fd_ioctl = connectControl(&g_ClientID);
    if (fd_ioctl<0) {
	fprintf(stderr, "Unable to open ioctl\n");
	return NULL;
    }

    //printf("[THREAD_GC] Connect OK :-)\n");

    if (pthread_create(&thd_gc_out, NULL, thread_GuestControl_out, (void *)fd_ioctl)) {
	fprintf(stderr, "Unable to start GuestControl_out thread\n");
  	return NULL;
    }

    for (;;) {
	int mtype, nparams;
	if (getNextHostMsg(fd_ioctl, g_ClientID, &mtype, &nparams)==0) {
            //printf("[THREAD_GC] Message type=%d Num params=%d\n", mtype, nparams);
	    switch (mtype) {
	     case HOST_CANCEL_PENDING_WAITS:
		printf("[THREAD_GC] Type=Cancel Pending Waits\n");
		break;
	     case HOST_EXEC_CMD:
		//printf("[THREAD_GC] Type=Execute Command\n");
		handleHostExecCmd(fd_ioctl, g_ClientID, nparams);
		break;
	     case HOST_EXEC_SET_INPUT:
		//printf("[THREAD_GC] Type=Set Input\n");
		handleGetCmdInput(fd_ioctl, g_ClientID, nparams);
		break;
	     case HOST_EXEC_GET_OUTPUT:
		//printf("[THREAD_GC] Type=Get Output\n");
		handleGetCmdOutput(fd_ioctl, g_ClientID, nparams);
		break;
	     default:
		printf("[THREAD_GC] Unknown type\n");
		break;
	    }
	}
	else {
	    printf("[THREAD_GC] Error getting next host message\n");
        }
    }
    return NULL;
}

void *thread_Connection_in(void *p_arg)
{
    struct thread_conn_info *p_tci = (struct thread_conn_info *)p_arg;
#define BUFFER_IN_SIZE 1048576
    char *buff_in;
    int nread;
    struct th_message *m;


    if (!p_tci) {
	fprintf(stderr, "[THREAD_CONN_IN] Error arg is NULL !\n");
	return NULL;
    }

    buff_in = (char *)malloc(BUFFER_IN_SIZE);
    if (!buff_in) {
	fprintf(stderr, "[THREAD_CONN_IN] socket=%d Unable to alloc buffer size %d\n", p_tci->socket, BUFFER_IN_SIZE);
	return NULL;
    }

    while ((nread = read(p_tci->socket, buff_in, BUFFER_IN_SIZE))>0) {
	//printf("[THREAD_CONN_IN %d] I've read %d bytes\n", p_tci->socket, nread);
	m = (struct th_message *)malloc(sizeof(struct th_message));
	if (!m) {
	    fprintf(stderr, "[THREAD_CONN_IN] malloc error\n");
	    continue;
	}
	m->cid = p_tci->socket;
	m->size = nread;
	m->term = 0;
	m->datas = (char *)malloc(nread);
	if (!m->datas) {
	    fprintf(stderr, "[THREAD_CONN_IN] malloc error on %d bytes\n", nread);
	    free(m);
	    continue;
	}
	memcpy(m->datas, buff_in, nread);
	//printf("[THREAD_CONN_ID %d] Pushing message out\n", p_tci->socket);
	push_message_out(m);
	//printf("[THREAD_CONN_ID %d] Ready to read\n", p_tci->socket);
    }

    fprintf(stderr, "[THREAD_CONN_IN] Thread to terminate because nread=%d\n", nread);
    m = (struct th_message *)malloc(sizeof(struct th_message));
    if (!m) {
	fprintf(stderr, "[THREAD_CONN_IN] malloc error\n");
	return NULL;
    }
    m->cid = p_tci->socket;
    m->size = 0;
    m->datas = NULL;
    push_message_out(m);

    return NULL;
}

void *thread_Connection_out(void *p_arg)
{
    struct thread_conn_info *p_tci = (struct thread_conn_info *)p_arg;

    for (;;) {
	struct th_message *msg;
	int nwrite,towrite;

	msg = pop_message_in(p_tci);
	if (!msg) {
	    fprintf(stderr, "pop_message_in() returned NULL pointer ; abnormal thing...\n");
	    continue;
	}
	//printf("[THREAD_CONN_OUT] Got message with datas size %d for pid %d\n", msg->size, msg->cid);

	if (msg->term) {
	    close(p_tci->socket);
	    del_thread_conn_info(p_tci);

	    return NULL;
	}

	towrite = msg->size;
	nwrite = 0;
	while (towrite>0) {
	    int w;

	    w = write(p_tci->socket, msg->datas+nwrite, towrite);
	    if (w<0) {
		fprintf(stderr, "write() returned %d, errno=%d\n", w, errno);
		break;
	    }
	    if (w==0) {	//EOF
		fprintf(stderr, "End of file on write() for pid %d\n", p_tci->socket);
		free(msg->datas);
		free(msg);
		return NULL;
	    }
	    towrite -= w;
	    nwrite += w;  
	}
	free(msg->datas);
	free(msg);
    }
}


int main(int argc, char *argv[]) 
{
    int srv_socket;
#ifdef USE_UNIX_SOCKET
    struct sockaddr_un srvaddr;
#else
    struct sockaddr_in srvaddr;
#endif
    pthread_t thd_gc = NULL;
    int one=1;

    pthread_mutex_init(&ctrl_message_out_mutex, NULL);
    pthread_cond_init(&ctrl_message_out_cond, NULL);

#ifdef USE_UNIX_SOCKET
    srv_socket = socket(AF_UNIX, SOCK_STREAM, 0);
#else
    srv_socket = socket(AF_INET, SOCK_STREAM, 0);
#endif
    if (srv_socket<0) {
	fprintf(stderr, "Unable to create server socket\n");
	exit(-1);
    }

#ifndef USE_UNIX_SOCKET
    setsockopt(srv_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
#endif

#ifdef USE_UNIX_SOCKET
    srvaddr.sun_family = AF_UNIX;
    sprintf(srvaddr.sun_path, "/data/system/.virt-%d", LISTEN_TCP_PORT);
#else
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(LISTEN_TCP_PORT);
    srvaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    memset(srvaddr.sin_zero, 0, sizeof(srvaddr.sin_zero));
#endif

    if (bind(srv_socket, (struct sockaddr *)&srvaddr, sizeof(srvaddr))<0) {
	fprintf(stderr, "Unable to bind server socket, errno=%d\n", errno);
        exit(-1);
    }
    if (listen(srv_socket, 5)<0) {
	fprintf(stderr, "Unable to listen on server socket, errno=%d\n", errno);
        exit(-1);
    }
    printf("Binding/Listen OK :-)\n");

    // Start Thread GC
    if (pthread_create(&thd_gc, NULL, thread_GuestControl, NULL)) {
	fprintf(stderr, "Unable to start GuestControl thread\n");
	exit(-1);
    }

    for (;;) {
	int cli_socket;

        if ((cli_socket = accept(srv_socket, NULL, 0))>=0) {
	    struct thread_conn_info *p_tci;
	    struct th_message *m;

	    printf("Got new connection id=%d!\n",cli_socket);
	    p_tci = (struct thread_conn_info * )malloc(sizeof(struct thread_conn_info));
	    if (!p_tci) {
		fprintf(stderr, "Error malloc...\n");
		continue;
	    }
	    p_tci->socket = cli_socket;
	    p_tci->ctrl_message_in = NULL;
	    pthread_mutex_init(&p_tci->ctrl_message_in_mutex, NULL);
	    pthread_cond_init(&p_tci->ctrl_message_in_cond, NULL);
	    add_thread_conn_info(p_tci);

	    m = (struct th_message *)malloc(sizeof(struct th_message));
	    if (!m) {
		fprintf(stderr, "Error malloc th_message\n");
		continue;
	    }
	    m->cid = 20000;
	    m->size = sizeof(int);
	    m->term = 0;
	    m->datas = (char *)malloc(sizeof(int));
	    *((int *)(m->datas)) = cli_socket;
	    //printf("Pushing message out\n");
	    push_message_out(m);

	    //printf("Creating threads\n");
	    if (pthread_create(&p_tci->thd_in, NULL, thread_Connection_in, (void *)p_tci)) {
		fprintf(stderr, "Unable to start Connection_in thread for socket %d\n", cli_socket);
		del_thread_conn_info(p_tci);
		free(p_tci);
		continue;
	    }
	    if (pthread_create(&p_tci->thd_out, NULL, thread_Connection_out, (void *)p_tci)) {
		fprintf(stderr, "Unable to start Connection_in thread for socket %d\n", cli_socket);
		del_thread_conn_info(p_tci);
		free(p_tci);
		continue;
	    }
	}
    }

    return 0;
}
