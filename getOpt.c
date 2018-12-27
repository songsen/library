#include <stdio.h>
#include <string.h>


char* get_opt( const char *opts)
{
#define BUFF_SIZE 512
#define configFIle (const char *)("lcu.conf")

    static char buff[BUFF_SIZE];
    bzero(buff,BUFF_SIZE);
	
    FILE *conf = fopen(configFIle,"r");
    if(conf ==NULL){
        printf("open config err!!!");
       // exit(-1); //打开配置文件失败
	   return "-1";
    }

    while(fgets(buff,BUFF_SIZE,conf)!= NULL){
        if(strcmp(buff,"\n")==0)
            continue;
        char *para = strtok(buff,"=\n\r");
        if(para == NULL)
            continue;
        do{
            if( para[0] =='#')
                break;

            if(strcmp(para,opts)==0){
                para = strtok(NULL,"=\n\r");
                if(para != NULL){
                    fclose(conf);
                    return para;
                };
            }
            
        }while((para = strtok(NULL,"=\n\r")) != NULL );

        bzero(buff,BUFF_SIZE);
    }

    fclose(conf);
    return "-1";
}

#include <stdlib.h>

int main(int argc, char const *argv[])
{
    /* code */
    if(argc < 2)
        return -1;


    char lcu_num[16];
	bzero(lcu_num,16);
	sprintf(lcu_num,"LCU%d",atoi(argv[1]));
    
    char *cmd = get_opt(lcu_num);

    if(atoi(cmd)<0){
        printf("not found parameter");
    }else{
        printf("%s",cmd);
    }
    return 0;
}


