/**
 * 获取Linux挂载设备信息
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <utmp.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <sys/statfs.h>

static const char *ignore_fs[] ={
    "none", "proc", "sysfs", "devpts", "usbfs", "usbdevfs", NULL};

typedef struct devInfo_t devInfo_t;
struct devInfo_t
{
    char source[256];
    unsigned long totalSize;
    unsigned long availSize;
};

static unsigned long kScale(unsigned long b, unsigned long bs)
{
    return (b * (unsigned long long) bs + 1024/2) / 1024;
}

int getDevMnt(char *target, devInfo_t *pu)
{
    int flag = 0;
    char **p;
    char *table ="/proc/mounts"; //这个可以根据自己的需要修改
    char buf[BUFSIZ];
    memset (buf, 0, BUFSIZ);
    struct mntent *mnt;
    struct statvfs statbuf;
    FILE*fp;

    fp = setmntent(table, "r");
    if(fp == NULL)
    {
        return -1;
    }

    while((mnt = getmntent(fp)))
    {
        for(p =(char **)ignore_fs; *p; p++)
        {
            if(!strcmp(mnt->mnt_fsname, *p))
            {
                flag =1;
                break;
            }
        }
        if(flag)
        {
            flag =0;
            continue;
        }
        if(0 > statvfs(mnt->mnt_dir, &statbuf))
        {
            return -1;
        }
        else
        {
            if(!strcmp(mnt->mnt_dir, target))
            {
                sprintf(pu->source, "%s", mnt->mnt_fsname);
                pu->availSize = kScale(statbuf.f_bavail, statbuf.f_bsize);
                pu->totalSize = kScale(statbuf.f_blocks, statbuf.f_bsize);
                printf("getDevMnt(). %s---%9lu---%9lu\n",
                        mnt->mnt_fsname, pu->totalSize, pu->availSize);
            }
        }
    }
    endmntent (fp);
    return 0;
}

#ifdef DBUG
/* test */
int main(int argc, char *argv[])
{
    devInfo_t pu;
    /*argv[1] mount node */
    getDevMnt(argv[1], &pu);

    return 0;
}
#endif
