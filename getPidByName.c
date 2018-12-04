/**
 * 通过名字来获取进程PID
 * */

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#define dprintf printf;

int getPidByName(const char *ps,const char *name)
{
    FILE *fp;
    char *p;
    char buf[256];
    int col = 0;


    fp = popen(ps,"r");

    if(fgets(buf,256,fp)!= NULL){
        p = strtok(buf," ");
        while(p != NULL){
            col++;
            if(strstr(p,"PID") != NULL){
                break;
            }
            p = strtok(NULL," ");
        }
    }

    if(p == NULL){
        printf("404 not find\n");
        return -1;
    }

    printf("PID column=%d\n",col);
    
    while(fgets(buf,256,fp) != NULL){
        printf("%s\n",buf);
        p = strstr(buf,name);
        if(NULL != p)
            break;
        p = NULL;
    }
    if(NULL == p){
        pclose(fp);
        return 0;
    }

    p = strtok(buf," ");
    while(--col){
        if((p = strtok(NULL, " ")) == NULL)
            break;
    }
    if( p != NULL)
        return atoi(p);

    return 0;
}

#ifdef DBUG
/* test */
int main(int argc, char const *argv[])
{
    /* code */
    const char *p = argv[0];
    if(argc == 2)
        p = argv[1];

    printf("argc = %d , get name %s \n",argc,argv[1]);
        
    printf("%s , pid = %d\n",argv[0],getPidByName("ps -x","/usr/share/code/resources/app/out/bootstrap-fork"));
    return 0;
}
#endif
