#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(int argc,char **argv){
    int clientFd;
    struct sockaddr_in clientAddr;
    socklen_t clientAddr_len;
    char server_addr[16];
    unsigned short server_port;
    char recBuf[4096];

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

    memset(recBuf,0,sizeof(recBuf));
    recv(clientFd,recBuf,sizeof(recBuf),0);
    printf("%s\n",recBuf);

    close(clientFd);
    return 0;
}
