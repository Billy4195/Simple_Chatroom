#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "user.h"

#define LISTENQ 10
#define HELLO_TO_EXISTED "[Server] anonymous is coming!\n"
#define HELLO_TO_NEW "[Server] Hello, anonymous! From: %s/%u\n"

int MAX(int x,int y){
    return x > y ? x : y;
}

int* set_fdset(user_t *curNode,fd_set *rfds,int *maxfdp){
    while(curNode != NULL){
        FD_SET(curNode->fd,rfds);
        *maxfdp = MAX(*maxfdp,curNode->fd);
        curNode = curNode->next;
    }
}

int main(){
    int listenFd;
    struct sockaddr_in serverAddr;
    socklen_t serverAddrLen;
    char helloMsg[60];
    user_t *root=NULL,*newUser,*cur=NULL;
    int connfd;
    struct sockaddr_in clientAddr;
    fd_set rfds;
    char recBuf[4096];
    size_t recLen=4096;
    int maxfdp=0;

    FD_ZERO(&rfds);

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
        FD_SET(listenFd,&rfds);
        maxfdp = MAX(maxfdp,listenFd);
        set_fdset(root,&rfds,&maxfdp);
        select(maxfdp+1,&rfds,NULL,NULL,NULL);
        if(FD_ISSET(listenFd,&rfds)){
            socklen_t clientAddrLen=sizeof(clientAddr);
            if( (connfd = accept(listenFd,(struct sockaddr*)NULL,NULL)) != -1 ){
                if(getpeername(connfd,(struct sockaddr*)&clientAddr,&clientAddrLen) == 0){
                    newUser = NewUser(connfd,inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port),"anonymous");
                    if(newUser != NULL){
                        sprintf(helloMsg,HELLO_TO_NEW,newUser->addr,newUser->port);
                        cur = root;
                        while(cur != NULL){
                            write(cur->fd,HELLO_TO_EXISTED,strlen(HELLO_TO_EXISTED));
                            cur = cur->next;
                        }
                        root = AddUserToList(root,newUser);
                        write(connfd,helloMsg,strlen(helloMsg));
                        continue;
                    }
                }
            }
            printf("[Error] create new user failed\n");
        }else{
            cur = root;
            while(cur != NULL){
                if(FD_ISSET(cur->fd,&rfds)){
                    if((recLen = read(cur->fd,recBuf,recLen)) == 0){
                        user_t *temp = cur;
                        sprintf(recBuf,"[Server] %s is offline.\n",cur->name);
                        close(cur->fd);
                        root = RemoveUserFromList(root,cur);
                        {// notify online user someone is offline
                            user_t *temp=root;
                            while(temp != NULL){
                                write(temp->fd,recBuf,strlen(recBuf));
                                temp = temp->next;
                            }
                        }
                        cur = cur->next;
                    }else{
                        recBuf[strlen(recBuf)-1] = '\0';
                        printf("%s\n",recBuf);
                    }
                    break;
                }else{
                    cur = cur->next;
                }
            }
        }
    }
    close(listenFd);

    return 0;
}
