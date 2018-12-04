#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/inotify.h>

#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <termios.h>
#include <string.h>

#include <signal.h>

#include "iomodel.h"

void watch_inotify(int fd, void *arg)
{

    char data[255] = {0};
    int num = 0;
    int *wfd = arg;

    printf("watch fd=%d, reg fd=%d", *wfd, fd);
    num = read(fd, data, 255); //没打开，那么就会阻塞
    printf("%d \n", num);      //如果打开，那么会继续跑到a这里

    for (int i = 0; i < num; i++)
    {
        printf("%d", data[i]);
    }

    printf("\n%s\n", &data[16]);
}

void wath_input(int fd, void *arg)
{
    char buf[256];

    int i = read(0, buf, sizeof(buf));
    buf[i] = 0;
    if (i > 0)
    {
        //printf("%c\n", buf[0]);
        write(1, buf, strlen(buf));
    }
}

void key_board(int fd, void *arg)
{
    struct input_event event_kb;
    if (read(fd, &event_kb, sizeof(event_kb)) == sizeof(event_kb))
    {
        if (event_kb.type == EV_KEY)
        {
            //if (event_kb.value == 0 || event_kb.value == 1)//1表示按下，0表示释放，会检测到两次
            if (event_kb.value == 1) //键按下
            {
                //printf("key %d %s\n", event_kb.code, (event_kb.value) ? "Pressed" : "Released");
                if (event_kb.code == KEY_ESC)
                    return;
                if (event_kb.code == KEY_Q)
                    printf("q\n");
                if (event_kb.code == KEY_W)
                    printf("w\n");
                if (event_kb.code == KEY_E)
                    printf("e\n");
                if (event_kb.code == KEY_R)
                    printf("r\n");
            }
        }
    }
}

void mouse_click(int fd_mouse ,void *arg)
{
    struct input_event event_mouse;  	  
    if (read(fd_mouse, &event_mouse, sizeof(event_mouse)) == sizeof(event_mouse))
    {
        if (event_mouse.type = EV_REL)
        {
            if (event_mouse.code == REL_WHEEL)
                printf("REL_WHEEL %d\n", event_mouse.value); //-1表示下滑，1表示上滑
        }

        if (event_mouse.code == BTN_LEFT && event_mouse.value == 1) //左键按下，1表示按下，0表示释放。不然会检测到两次
            printf("left down: %d\n", event_mouse.code);

        if (event_mouse.code == BTN_RIGHT && event_mouse.value == 1)
            printf("right down: %d\n", event_mouse.code);

        if (event_mouse.code == BTN_MIDDLE && event_mouse.value == 1)
            printf("middle down: %d\n", event_mouse.code);
    }
}

void mouse_point(int fd, void *arg)
{
    char buf[6];  
    // 读取鼠标设备中的数据    
    if(read(fd, buf, 6) <= 0) {    
        return;    
    }    
    // 打印出从鼠标设备中读取到的数据    
    printf("Button type = %d, X = %d, Y = %d, Z = %d\n", (buf[0] & 0x07), buf[1], buf[2],   buf[3]);    
}


void signal_hander(int sig)
{
    printf("exit listen ....\n");
    exit_listen();
}

int main()
{
    int fd = 0;
    int wfd = 0;

    signal(SIGTERM,signal_hander);

    //监听注册设备
    fd = inotify_init();                             //初始化fd
    wfd = inotify_add_watch(fd, "/dev/", IN_CREATE); //监听文件是否打开
    register_event(fd, watch_inotify, (void *)&wfd);

    //异步IO
    // fcntl(0,__F_SETOWN,getpid());
    // int fd_flags = fcntl(0,F_GETFL);
    // fcntl(0,F_SETFL,(fd_flags|O_ASYNC));

    //流
    struct termios term_orig, term_vi;
    tcgetattr(0, &term_orig);
    term_vi = term_orig;
    term_vi.c_lflag &= (~ICANON & ~ECHO); // leave ISIG ON- allow intr's
    term_vi.c_iflag &= (~IXON & ~ICRNL);
    term_vi.c_oflag &= (~ONLCR);
    // #ifndef 0
    // 	term_vi.c_cc[VMIN] = 1;
    // 	term_vi.c_cc[VTIME] = 0;
    // #endif
    //tcsetattr(0, TCSANOW, &term_vi);

    int fd_kb;
    fd_kb = open("/dev/input/event1", O_RDONLY); //键盘输入
    if (fd_kb <= 0)
    {
        printf("open device error\n");
        return 0;
    }
    register_event(fd_kb, key_board, NULL);
    //register_event(0, wath_input, NULL);


    // 打开鼠标设备    
    fd = open( "/dev/input/mice", O_RDONLY );    
    // 判断是否打开成功    
    if(fd<0) {    
        printf("Failed to open \"/dev/input/mice\".\n");    
        exit(1);    
    } else {    
        printf("open \"/dev/input/mice\" successfuly.\n");    
    }  
    register_event(fd,mouse_point,NULL);

    int fd_mouse;    	  
	fd_mouse = open("/dev/input/event4", O_RDONLY); //鼠标click  
	if(fd_mouse <= 0)  
	{  
        printf("open device error\n");  
        return 0;  
	}
    register_event(fd_mouse,mouse_click,NULL);

    run_listen();


    close(fd);  
	close(fd_kb); 
	close(fd_mouse);

    // strcpy(buf, "\033[4;0Haaaa");
    // write(1, buf, strlen(buf));
    // tcsetattr(0, TCSANOW, &term_orig);
    return 0;
}
