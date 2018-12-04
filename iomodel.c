/**
 * select 的事件监听注册封装函数
 * */

#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "iomodel.h"

typedef struct mode_fd_set {
    int fd;                     //监听的IO
    void (*proc)(int ,void *);  //处理函数
    void *arg;
} mode_fd_set_t;


static fd_set rfds;                        /*select 监听可读集合*/
static int allconnfd = -1;                      /*目前监听的句柄数量*/
static int maxfd ;                         /*最大fd */
static int maxi;                           /*最大数组*/
static mode_fd_set_t client[FD_SETSIZE];   /*监听数组*/
static struct timeval *timeout;
static int flag;

/**
 * 
 * */
int static init_mode()
{
    int i=0;
    for(i = 0;i <FD_SETSIZE;i++){
        client[i].fd = -1;
        client[i].proc = NULL;
    }
    FD_ZERO(&rfds);
    allconnfd = 0;
    maxfd = -1;
    maxi = 0;
    timeout = NULL;
    flag = 1;
}

/**
 * 
 * */
int register_event(int fd, void (*proc)(int ,void *),void *arg)
{
    if(allconnfd == -1){
        init_mode();
    }

    if(maxfd < fd)
        maxfd = fd;

    int i=0;
    while(i < FD_SETSIZE){
        if(client[i].fd == -1){
            client[i].fd = fd;
            client[i].proc = proc;
            client[i].arg = arg;
            FD_SET(fd,&rfds);
            allconnfd++;
            if (i > maxi)
				maxi = i;				/* max index in client[] array */
            break;
        } 
        if(++i == FD_SETSIZE)
            return -1;
    }
    return 0;
}
/**
 * 
 * */
int mask_event(int fd)
{
    int i=0;
    while(i < maxi){
        if(client[i].fd == fd){
            FD_CLR(fd,&rfds);
            client[i].fd = -1;
            allconnfd--;
            return 0;
        }    
    }
    return -1;
}
/**
 * 
 * */
int remove_event(int fd)
{
    return -1;
}

/**
 * 
 * */
 void set_timeout(struct timeval *tv)
 {
     timeout = tv;
 }

/**
 * 
 * */
int run_listen()
{
    fd_set rset;
    struct timeval *interval ;
    int nready;

    while(flag){
        interval = timeout;
        rset = rfds;
        nready = select(maxfd+1,&rset,NULL,NULL,interval);
        if(nready < 0){
            if(errno == EINTR)
                continue;
        }else if(nready == 0){
            //select timeout
            continue;
        }
        int i=0;
        int fd;
        for(;i<maxi+1;i++){
            if( (fd = client[i].fd) < 0)
                continue;
            if(FD_ISSET(fd,&rset)){
                client[i].proc(fd,client[i].arg);
                if(--nready <= 0)
                    break;
            }
        }
    }
}

void exit_listen()
{
    flag = 0;
}

