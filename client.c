#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int MAX(int x,int y){
        return x > y ? x : y;
}

int main(int argc,char **argv){
    int clientFd;
    struct sockaddr_in clientAddr;
    socklen_t clientAddr_len;
    char server_addr[16];
    unsigned short server_port;
    char recBuf[4096];
    int recCount;
    fd_set rfds;
    unsigned short maxfdp;

    FD_ZERO(&rfds);

    if(argc == 3){
        strcpy(server_addr,argv[1]);
        server_port = atoi(argv[2]);
        printf("Connect to %s:%u\n",server_addr,server_port);
    }else{
        strcpy(server_addr,"127.0.0.1");
        server_port = 3000;
    }

    clientFd = socket(AF_INET,SOCK_STREAM,0);
    clientAddr_len = sizeof(clientAddr);

    memset(&clientAddr,0,clientAddr_len);
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = inet_addr(server_addr);
    clientAddr.sin_port = htons(server_port);

    connect(clientFd,(struct sockaddr*)&clientAddr,sizeof(clientAddr));
    while(1){
        memset(recBuf,0,sizeof(recBuf));
        FD_SET(0,&rfds);
        FD_SET(clientFd,&rfds);
        maxfdp = MAX(0,clientFd) + 1;
        select(maxfdp,&rfds,NULL,NULL,NULL);
        if(FD_ISSET(0,&rfds)){
            recCount = read(0,recBuf,sizeof(recBuf));
            if(strcmp(recBuf,"exit\n") == 0){
                break;
            }else{
                write(clientFd,recBuf,sizeof(recBuf));
            }
        }else if(FD_ISSET(clientFd,&rfds)){
            recCount = read(clientFd,recBuf,sizeof(recBuf));
            if(recCount > 0){
                printf("%s",recBuf);
            }
        }
    }

    close(clientFd);
    return 0;
}
