#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define LISTENQ 10
#define HELLO_TO_EXISTED "[Server] anonymous is coming!\n"
#define HELLO_TO_NEW "[Server] Hello, anonymous! From: %s/%u\n"

int main(){
    int listenFd;
    struct sockaddr_in serverAddr;
    socklen_t serverAddrLen;
    char helloMsg[60];

    listenFd = socket(AF_INET,SOCK_STREAM,0);
    serverAddrLen = sizeof(serverAddr);
    memset(&serverAddr,0,serverAddrLen);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(0);

    bind(listenFd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    if(getsockname(listenFd,(struct sockaddr*)&serverAddr,(socklen_t*)&serverAddrLen) == 0){
        printf("Binding on: %s:%u\n",inet_ntoa(serverAddr.sin_addr),ntohs(serverAddr.sin_port));
    }else{
        fprintf(stderr,"Binding error: %d\n",errno);
    }

    listen(listenFd,LISTENQ);
    
    while(1){
        int connfd;
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen=sizeof(clientAddr);
        if( (connfd = accept(listenFd,(struct sockaddr*)NULL,NULL)) != -1 ){
            if(getpeername(connfd,(struct sockaddr*)&clientAddr,&clientAddrLen) == 0){
                printf(HELLO_TO_EXISTED);
                sprintf(helloMsg,HELLO_TO_NEW,inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
            }
        }
        write(connfd,helloMsg,strlen(helloMsg));
        close(connfd);
    }

    return 0;
}
