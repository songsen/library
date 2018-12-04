
/*添加监听句柄和处理函数*/
int register_event(int fd, void (*proc)(int ,void *),void *arg);
/*屏蔽监听句柄*/
int mask_event(int fd);
/*执行监听*/
int run_listen();
/*set timeout */
void set_timeout(struct timeval *tv);
/* 退出监听 */
void exit_listen();