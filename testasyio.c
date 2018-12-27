#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/socket.h>

int main()
{
    struct termios term_orig, term_vi;
    tcgetattr(0, &term_orig);
    term_vi = term_orig;
    term_vi.c_lflag &= (~ICANON & ~ECHO); // leave ISIG ON- allow intr's
    term_vi.c_iflag &= (~IXON & ~ICRNL);
    term_vi.c_oflag &= (~ONLCR);
    term_vi.c_cc[VMIN] = 3;
    term_vi.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &term_vi);

    char *buf[256];
    read(0,buf,sizeof(buf));
    write(1,buf,sizeof(buf));

}



